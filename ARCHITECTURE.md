# ARCHITECTURE

> 🧠 Documentation enrichie par IA — basée sur l’analyse du code, de la configuration CMake et des sous-modules ArcGeoSim/ArcaneInfra

## 1. Vue d’ensemble

ShArc est une application de simulation numérique basée sur le framework **Arcane/ArcGeoSim**.  
L’architecture se décompose en trois couches principales :

1. **Infrastructure de simulation** (Arcane, ArcGeoSim, ArcaneInfra, ArcRes) — sous-modules externalisés.
2. **Bibliothèque ShArc** (`src/`) — modules métier et infrastructure applicative.
3. **Cas de test et scénarios** (`test/`) — fichiers `.arc` + `ShArc.config` pilotant les simulations.

La bibliothèque ShArc est construite comme une **librairie unique** (`ShArc`) qui regroupe :

- un module d’orchestration générique (`ShArc`),
- un module de simulation diphasique et thermo‑chimique (`TwoPhaseFlowSimulation` / `ThermoChemicalConvection`),
- une bibliothèque de solveurs éléments‑finis (`CAWF`),
- un module de benchmark géométrique (`ExaDiBench`),
- un exécutable d’entrée unique (`ShArc.exe`).

## 2. Organisation du dépôt

### 2.1 Racine

- `CMakeLists.txt` — point d’entrée build, intégration ArcGeoSim via `LoadArcGeoSim.cmake`.
- `CMake/` — modules CMake auxiliaires (FindMetis, FindNvAMG, etc.).
- `common/` :
  - `ArcGeoSim/` — framework ArcGeoSim (submodule).
  - `ArcaneInfra/` — outils build/test Arcane (submodule).
- `docs/` — site Hugo Book + Doxygen.
- `src/` — bibliothèque ShArc (voir ci-dessous).
- `test/` — cas de référence et tests CTest.
- `README.md` — installation et guides build.

### 2.2 Bibliothèque ShArc (`src/`)

- `src/CMakeLists.txt`  
  Crée la bibliothèque `ShArc` et la relie à :
  - `arcres` (modèle de données physiques),
  - `CAWF` (solveurs FEM),
  - bibliothèques ArcGeoSim (mesh/physics/tests, surtout pour Windows).
- `src/config.xml`  
  Déclare les sous-répertoires gérés (ShArc, TwoPhaseFlowSimulation, ExaDiBench) pour ArcaneInfra.

Sous-répertoires principaux :

- `src/ShArc/` — module d’orchestration global.
- `src/TwoPhaseFlowSimulation/` — solveur diphasique et module thermo‑chimique.
- `src/CAWF/` — bibliothèque CAWF (Poisson, Elasticity, Compaction, FEMUtils).
- `src/ExaDiBench/` — module de benchmark géométrique.
- `src/Main/` — exécutable `ShArc.exe`.

## 3. Pile logicielle de simulation

### 3.1 Infrastructure Arcane / ArcGeoSim

La base de la simulation est fournie par Arcane/ArcGeoSim :

- **Arcane** :
  - gestion du maillage (`IMesh`),
  - boucle en temps (`ITimeLoopMng`),
  - variables, I/O, parallelisme (`IParallelMng`).
- **ArcGeoSim** :
  - services applicatifs (`AppService`),
  - géométrie (`IGeometryMng`, `IGeometry`),
  - opérateurs numériques (schémas deux‑points, opérateurs `DivKGrad`, etc.),
  - time line (`ITimeLine`, politique de pas de temps),
  - module de validation (`IServiceValidator`).
- **ArcRes** :
  - modèle de données physiques : systèmes fluide/solide, propriétés (`Pressure`, `Saturation`, `Density`, etc.).
- **Alien / solveurs linéaires** :
  - bibliothèques linéaires (Hypre, PETSc, Trilinos, etc.) intégrées via CAWF ou ArcNum.

ShArc se positionne **au-dessus** de cette pile, en fournissant des modules métier qui consomment ces services.

### 3.2 Bibliothèque ShArc (vue modulaire)

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
```

- `src/ShArc/` :
  - Module d’orchestration qui gère :
    - timeline & pas de temps (`ITimeLine`, `TimeStep::IComputer`),
    - géométrie (`IGeometryMng`),
    - post‑traitement (writers, fréquence de sortie),
    - validation finale (`IServiceValidator`).
- `src/TwoPhaseFlowSimulation/` :
  - `TwoPhaseFlowSimulationModule` :
    - système non linéaire pression/saturations (écoulements diphasiques),
    - assemblage de la matrice et du résidu (accumulation, flux internes/bord/puits, fermeture),
    - contraintes physiques (saturations dans [0,1], somme = 1).
  - `ThermoChemicalConvectionModule` :
    - couplage pression–concentration–température (P–C–T),
    - termes d’advection, diffusion, conduction couplés fluide/solide.
  - Dossiers associés :
    - `PhysicalSystem/` (interfaces ArcRes : `IPhysicalSystem`, `ISystemBuilder`, `IVariableManager`),
    - `PhysicalLaws/` (lois capillaire, densité, perméabilité relative, constantes),
    - `BoundaryCondition/`, `InitialCondition/`, `WellCondition/` (CL, CI, puits).
- `src/CAWF/` :
  - Regroupe des solveurs FEM génériques :
    - `Poisson/`, `Elasticity/`, `Compaction/`, `DynamicMeshMng/`.
  - `FEMUtils/` :
    - gestion des systèmes linéaires FEM (`DoFLinearSystem`, backends Alien/Hypre/PETSc/Trilinos),
    - formats de matrices (CSR, BSR, COO),
    - helpers de conditions aux limites,
    - support GPU/SYCL (`ALIEN_USE_SYCL`).
- `src/ExaDiBench/` :
  - Module de benchmark géométrique :
    - export maillage (VTK/MSH/IXM),
    - création de groupes de bord (`BorderNodes`, `OuterFaces`),
    - calcul volumes/surfaces/centres,
    - déformation pseudo‑aléatoire des nœuds intérieurs et recalcul des mesures.
- `src/Main/` :
  - Exécutable `ShArc.exe` :
    - `main()` minimal qui instancie `ArcGeoSim::main(info)`.

## 4. Flux d’exécution type

### 4.1 Lancement d’un cas `.arc`

1. L’utilisateur lance :

   ```bash
   ./ShArc.exe <mon-cas>.arc
   ```

2. Le fichier `.arc` :

   - référence une boucle en temps (ex. `ArcResTimeLoop`),
   - référence les modules à charger (`ShArc`, `TwoPhaseFlowSimulation`, `PhysicalModel`, etc.),
   - configure les services (CI/CL, lois, solveur Newton, solveur linéaire, post‑traitement).

3. `ShArc.exe` :

   - délègue à `ArcGeoSim::main()` la gestion de la boucle de simulation.
   - charge les modules définis dans `ShArc.config` et le cas `.arc`.

### 4.2 Rôle combiné `ShArc` + modules physiques

- `ShArcModule` :
  - `build()` : prépare maillage et connectivités.
  - `prepareInit()`, `init()`, `endInit()` : installe services (temps, géométrie, post‑traitement, validateurs).
  - `beginTimeStep()` / `endTimeStep()` : gère les sorties périodiques et les conditions d’arrêt.
  - `endSimulation()` : post‑traitement final + validation (`IServiceValidator`).

- `TwoPhaseFlowSimulationModule` / `ThermoChemicalConvectionModule` :
  - `init()` : construit le système physique ArcRes, les groupes, les variables, les lois, le solveur Newton.
  - `compute()` : appelle `newton()->solve()` et, en cas d’échec, déclenche un rollback (`ReplayException`).

Les tests (`test/`) instancient ces flux via des couples `ShArc.config` + fichiers `.arc`, pour différents scénarios (cas simples, SPE10, Poisson, Elasticity, Compaction, ExaDiBench, ThermoChemicalConvection).

## 5. Architecture de la documentation

La documentation est alignée sur cette architecture logicielle :

- READMEs au plus près des modules :
  - `src/`, `src/CAWF/`, `src/ShArc/`, `src/TwoPhaseFlowSimulation/`, `src/ExaDiBench/`, `src/Main/`.
  - `test/TwoPhaseFlowSimulation/`, `test/CAWF/`, `test/ExaDiBench/`, `test/ThermoChemicalConvection/`, `test/LargeScaleTwoPhaseFlowSimulation/`.
- Site Hugo Book (`docs/`) :
  - section `src/` → modules de la bibliothèque,
  - section `test/` → cas de référence,
  - section `api/` → Doxygen.
- Doxygen (`docs/public/api`) :
  - API détaillée des classes/fonctions C++ (Arcane/ArcGeoSim/CAWF/ShArc…).

Cette structuration permet d’avoir :
- une **vue macro** dans `ARCHITECTURE.md` et la page Hugo associée,
- une **vue détaillée** par module/cas via les READMEs,
- une **référence complète** via Doxygen.
