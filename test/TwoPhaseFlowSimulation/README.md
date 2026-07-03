# Tests TwoPhaseFlowSimulation

> 🧠 Documentation enrichie par IA — basée sur l'analyse de la nomenclature des cas `.arc`, des maillages et du module TwoPhaseFlowSimulation

## 📋 Table des matières

- [Description](#description)
- [Contenu](#contenu)
- [Objectifs des tests](#objectifs-des-tests)
- [Utilisation](#utilisation)
- [Architecture](#architecture)
- [Ressources](#ressources)

## Description

Le répertoire `test/TwoPhaseFlowSimulation/` regroupe les **cas de test de référence** pour le module `TwoPhaseFlowSimulation`. Il couvre :

- des cas synthétiques simples (maillage 10×1×1, variantes avec puits, pression capillaire),
- des cas de type **SPE10** (maillage hétérogène de grande taille),
- des variantes explorant différents solveurs et préconditionneurs linéaires (Hypre, PETSc, Trilinos/IFPS, MCGS, HTS, GPU/SYCL, etc.),
- des variantes physiques (avec/sans pression capillaire, avec/sans puits).

Ces cas sont utilisés pour la **validation physique**, la **comparaison numérique des solveurs** et la **détection de régressions** dans l’assemblage du système non linéaire (Newton).

## Contenu

### Maillages et configuration

- `mesh-10x1x1.vt2` — maillage simple 10×1×1 (cas de base).
- `mesh-well.vt2` — maillage simple avec puits.
- `SPE10-mesh-level0.vt2` — maillage SPE10 (niveau 0).
- `SPE10-mesh-level0-well.vt2` — maillage SPE10 avec puits.
- `ShArc.config` — configuration Arcane pour ces cas :
  - boucle de temps `ArcResTimeLoop`,
  - modules : `ShArc`, `TwoPhaseFlowSimulation`, `PhysicalModel`,
  - points d’entrée :
    - `TwoPhaseFlowSimulation.Init`, `ShArc.Init` (phase init),
    - `TwoPhaseFlowSimulation.Compute` (compute-loop).

### Cas `.arc` principaux

- **Cas de base** :
  - `use-case-two-phase-flow.arc`  
    Cas standard deux phases sur un maillage simple.
  - `use-case-two-phase-flow-with-capillary-pressure.arc`  
    Même cas avec pression capillaire activée.
  - `use-case-two-phase-flow-well.arc`  
    Cas deux phases avec puits.

- **Cas SPE10 (maillage hétérogène)** :
  - `use-case-two-phase-flow-spe10.arc`  
    Config de base SPE10 (sans puits).
  - `use-case-two-phase-flow-spe10-well.arc`  
    SPE10 avec puits.

### Variantes solveurs / préconditionneurs (SPE10)

Les suffixes des fichiers `.arc` indiquent le **solveur linéaire** et le **préconditionneur** utilisés dans le bloc `two-phase-flow-simulation > numerics > newton > linear-solver`.

Exemples (liste non exhaustive) :

- Hypre :
  - `use-case-two-phase-flow-spe10-hypre-bcgs-ilu0.arc`
- PETSc :
  - `use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc`
- IFPS / Trilinos :
  - `use-case-two-phase-flow-spe10-ifps-bcgs-ilu0.arc`
  - `use-case-two-phase-flow-spe10-ifps-bcgs-cpr.arc`
- MCGS :
  - `use-case-two-phase-flow-spe10-mcgs-bcgs-ilu0.arc`
  - `use-case-two-phase-flow-spe10-mcgs-bcgs-cpr.arc`
  - `use-case-two-phase-flow-spe10-mcgsgpu-bcgs-cpr.arc` (variante GPU)
- HTS :
  - `use-case-two-phase-flow-spe10-hts-bcgs-ilu0.arc`
  - `use-case-two-phase-flow-spe10-hts-bcgs-cpr.arc`
- Combinaisons « classiques » BCGS :
  - `use-case-two-phase-flow-spe10-bcgs-ilu0.arc`
  - `use-case-two-phase-flow-spe10-bcgs-cxr.arc`
  - `use-case-two-phase-flow-spe10-bcgs-cxr-sycl.arc` (variante SYCL)
- TRI :
  - `use-case-two-phase-flow-spe10-tri-bcgs-ilu0.arc`

### Fichiers de référence

- `RefUseCaseTwoPhaseFlow_DebugNewtonMatrix_1_1.txt`  
  Référence pour la matrice du système non linéaire (bloc Newton) pour un cas de base.
- `RefUseCaseTwoPhaseFlow_DebugNewtonVector_1_1.txt`  
  Référence pour le vecteur résidu correspondant.

Ces fichiers sont utilisés pour vérifier que l’assemblage du système (jacobien + résidu) reste **numériquement stable** au fil des modifications du module.

## Objectifs des tests

- **Validation physique** :
  - Vérifier la cohérence des résultats (pression, saturations) sur des maillages simples et SPE10.
  - Tester l’effet des puits (cas *well*) et de la pression capillaire (cas *with-capillary-pressure*).

- **Comparaison de solveurs** :
  - Comparer robustesse et performances de différentes combinaisons solveur/préconditionneur :
    - Hypre, PETSc, IFPS/Trilinos, MCGS, HTS, variantes GPU/SYCL, etc.
  - Évaluer l’impact du choix de solveur sur la convergence du Newton et le coût CPU.

- **Régression numérique** :
  - Utiliser les fichiers de référence Newton pour détecter les régressions dans l’assemblage du système :
    - structure de la matrice (profil, remplissage),
    - valeurs du résidu.

## Utilisation

Depuis le répertoire de build :

```bash
# Exécuter la suite de tests TwoPhaseFlowSimulation via CTest
ctest -R TwoPhaseFlowSimulation
```

Pour lancer un cas particulier :

```bash
# Exemple : cas SPE10 avec PETSc + BCGS + ILU0
./ShArc.exe test/TwoPhaseFlowSimulation/use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc
```

Adapter le nombre de processus MPI et les options en fonction de votre configuration CMake (compilateur MPI, solveurs externes disponibles).

## Architecture

```text
┌────────────────────────────────────────────────┐
│        test/TwoPhaseFlowSimulation/           │
├────────────────────────────────────────────────┤
│  ShArc.config                                 │
│  mesh-10x1x1.vt2                              │
│  mesh-well.vt2                                │
│  SPE10-mesh-level0.vt2                        │
│  SPE10-mesh-level0-well.vt2                   │
│  use-case-two-phase-flow*.arc                 │
│  RefUseCaseTwoPhaseFlow_DebugNewton*.txt      │
└────────────────────────────────────────────────┘
           │                       │
           ▼                       ▼
   src/TwoPhaseFlowSimulation   src/ShArc
   (module physique)           (orchestration)
```

## Ressources

- `src/TwoPhaseFlowSimulation/README.md` — documentation du module de simulation diphasique.
- `test/LargeScaleTwoPhaseFlowSimulation/README.md` — documentation des cas XXL (SPE10 grande échelle).
- Documentation Arcane/ArcGeoSim — pour la syntaxe des cas `.arc` et la configuration des solveurs linéaires.
- Documentation ArcRes — pour la signification des propriétés physiques (`Pressure`, `Saturation`, `Density`, etc.).
