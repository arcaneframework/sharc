# Tests TwoPhaseFlowSimulation

> 🧠 Documentation enrichie par IA — basée sur l'analyse de la structure des cas

## 📋 Table des matières

- [Description](#description)
- [Organisation des cas](#organisation-des-cas)
- [Cas de base](#cas-de-base)
- [Cas avec capillarité et puits](#cas-avec-capillarité-et-puits)
- [Cas SPE10 (benchmark)](#cas-spe10-benchmark)
- [Solveurs linéaires et préconditionneurs](#solveurs-linéaires-et-préconditionneurs)
- [Mesh et configuration commune](#mesh-et-configuration-commune)

## Description

Le répertoire `test/TwoPhaseFlowSimulation/` contient la suite de cas de test pour le module `src/TwoPhaseFlowSimulation/`. Ces cas couvrent :

- des scénarios de base de simulation diphasique,
- des cas avec pression capillaire,
- des cas avec puits (sources/sinks),
- un ensemble de variantes du benchmark SPE10 utilisant différents solveurs linéaires et préconditionneurs.

Les fichiers `.arc` décrivent chacun un cas Arcane/ShArc complet : maillage (`.vt2`), système physique, conditions initiales, conditions aux limites, puits, paramètres numériques (Newton, schéma à deux points, solveur linéaire).

## Organisation des cas

Contenu principal :

```text
test/TwoPhaseFlowSimulation/
  mesh-10x1x1.vt2                      # petit maillage de test
  mesh-well.vt2                        # maillage pour cas avec puits
  SPE10-mesh-level0.vt2                # maillage SPE10 (niveau 0)
  SPE10-mesh-level0-well.vt2           # maillage SPE10 avec puits

  ShArc.config                         # configuration ShArc utilisée par ces cas

  use-case-two-phase-flow.arc
  use-case-two-phase-flow-with-capillary-pressure.arc
  use-case-two-phase-flow-well.arc

  use-case-two-phase-flow-spe10.arc
  use-case-two-phase-flow-spe10-well.arc
  use-case-two-phase-flow-spe10-bcgs-ilu0.arc
  use-case-two-phase-flow-spe10-bcgs-cxr.arc
  use-case-two-phase-flow-spe10-bcgs-cxr-sycl.arc
  use-case-two-phase-flow-spe10-hts-bcgs-ilu0.arc
  use-case-two-phase-flow-spe10-hts-bcgs-cpr.arc
  use-case-two-phase-flow-spe10-hypre-bcgs-ilu0.arc
  use-case-two-phase-flow-spe10-ifps-bcgs-ilu0.arc
  use-case-two-phase-flow-spe10-ifps-bcgs-cpr.arc
  use-case-two-phase-flow-spe10-mcgs-bcgs-ilu0.arc
  use-case-two-phase-flow-spe10-mcgs-bcgs-cpr.arc
  use-case-two-phase-flow-spe10-mcgsgpu-bcgs-cpr.arc
  use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc
  use-case-two-phase-flow-spe10-tri-bcgs-ilu0.arc

  RefUseCaseTwoPhaseFlow_DebugNewtonMatrix_1_1.txt
  RefUseCaseTwoPhaseFlow_DebugNewtonVector_1_1.txt
```

## Cas de base

- `use-case-two-phase-flow.arc`  
  Cas de base de simulation diphasique sur `mesh-10x1x1.vt2`. Sert de test rapide pour le module `TwoPhaseFlowSimulation` sans capillarité ni puits.

- `use-case-two-phase-flow-with-capillary-pressure.arc`  
  Cas similaire au précédent, avec activation des lois de pression capillaire (`PhysicalLaws/CapillaryPressureLaw`). Permet de vérifier les contributions de capillarité et leur impact sur l’assemblage.

## Cas avec capillarité et puits

- `use-case-two-phase-flow-well.arc`  
  Cas de base avec puits sur `mesh-well.vt2`. Utilise le service `WellCondition/Well` pour tester la gestion des puits (débits ou pressions imposées), indices de puits, mise à jour des flux puits.

- `use-case-two-phase-flow-spe10-well.arc`  
  Variante SPE10 avec puits, sur `SPE10-mesh-level0-well.vt2`.

## Cas SPE10 (benchmark)

Les cas `use-case-two-phase-flow-spe10*.arc` utilisent la géométrie et les propriétés du benchmark SPE10 (réservoir hétérogène) pour tester le solveur sur un cas réaliste à maillage plus coûteux.

Cas génériques :

- `use-case-two-phase-flow-spe10.arc`  
  Cas de référence SPE10 avec configuration par défaut (maillage `SPE10-mesh-level0.vt2`, système diphasique, capillarité, etc.).

- `use-case-two-phase-flow-spe10-well.arc`  
  SPE10 avec puits (combinaison flux/pression, indices de puits, etc.).

## Solveurs linéaires et préconditionneurs

Les variantes autour de SPE10 exploitent différents solveurs linéaires, préconditionneurs et backends. La nomenclature des fichiers `.arc` suit le schéma :

```text
use-case-two-phase-flow-spe10-<backend>-<iterative>-<precond>.arc
```

où :

- `<backend>` : backend ou stack de solveur (hypre, petsc, ifps, mcgs, hts, tri, bcgs, etc.),
- `<iterative>` : méthode itérative principale (souvent `bcgs` pour BiCGStab),
- `<precond>` : préconditionneur (par ex. `ilu0`, `cpr`, `cxr`).

Exemples :

- `use-case-two-phase-flow-spe10-bcgs-ilu0.arc`  
  Solveur BiCGStab avec préconditionneur ILU(0) sur backend standard.

- `use-case-two-phase-flow-spe10-bcgs-cxr.arc`  
  Variante avec préconditionneur CXR (Cross Approximation ? selon l’implémentation de backend Alien/ArcGeoSim).

- `use-case-two-phase-flow-spe10-bcgs-cxr-sycl.arc`  
  Variante accélérée SYCL (GPU) pour CXR.

- `use-case-two-phase-flow-spe10-hts-bcgs-ilu0.arc` / `...-cpr.arc`  
  Cas utilisant un backend HTS (High-Throughput Solver) avec BiCGStab et préconditionneur ILU0 ou CPR.

- `use-case-two-phase-flow-spe10-hypre-bcgs-ilu0.arc`  
  Cas utilisant Hypre comme backend, BiCGStab + ILU0.

- `use-case-two-phase-flow-spe10-ifps-bcgs-ilu0.arc` / `...-cpr.arc`  
  Cas utilisant un backend IFPS (IfpSolver) avec BiCGStab et préconditionneur ILU0 ou CPR.

- `use-case-two-phase-flow-spe10-mcgs-bcgs-ilu0.arc` / `...-cpr.arc`  
  Cas utilisant un backend MCGS (Multi-grid / Multi-colour?) avec BiCGStab, ILU0 ou CPR.

- `use-case-two-phase-flow-spe10-mcgsgpu-bcgs-cpr.arc`  
  Variante GPU du backend MCGS avec CPR.

- `use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc`  
  Cas avec backend PETSc, BiCGStab + ILU0.

- `use-case-two-phase-flow-spe10-tri-bcgs-ilu0.arc`  
  Variante utilisant un backend Trilinos (tri*) avec BiCGStab + ILU0.

Ces cas permettent de comparer performances, robustesse et comportement des différents solveurs/préconditionneurs sur un benchmark commun.

## Mesh et configuration commune

  Maillage simple pour test rapide (10×1×1 cellules).

  Maillage avec groupe(s) adapté(s) aux puits (groupes de cellules/faces pour `WellCondition`).

  Maillages de base pour le benchmark SPE10, avec ou sans puits.

  Fichier de configuration ShArc commun aux cas, définissant les modules à charger (ShArc, TwoPhaseFlowSimulation, services de maillage, services de solveur, etc.) et les options globales de simulation/post-processing.





