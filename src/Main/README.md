# Main (ShArc.exe)

> 🧠 Documentation enrichie par IA — basée sur l'analyse du point d'entrée C++ et du CMake

## Description

Le répertoire `src/Main/` contient le **point d'entrée exécutable** de l'application ShArc. Il ne porte aucune logique métier : il instancie simplement l'application Arcane/ArcGeoSim et démarre la boucle de calcul en chargeant les modules (notamment `ShArc`, `TwoPhaseFlowSimulation`, `ThermoChemicalConvection`, etc.) décrits dans le fichier de cas `.arc`.

## Fonctionnalités

- ✅ Création de l'exécutable `ShArc.exe`.
- ✅ Intégration avec le lanceur générique ArcGeoSim (`ArcGeoSimMain`).
- ✅ Liaison avec la bibliothèque `ShArc` et les composants ArcGeoSim nécessaires.

## Prérequis

- Frameworks :
  - Arcane / ArcGeoSim (modules, services, time loops).
  - Bibliothèque ShArc construite (voir `src/` et README racine).

## Installation / Build

La définition CMake est donnée par `src/Main/CMakeLists.txt` :

```cmake
createExecutable(ShArc.exe)

addSources(ShArc.exe main.cc)

if(TARGET ArcGeoSim::arcgeosim)
  linkLibraries(ShArc.exe ShArc ArcGeoSim::arcgeosim)
else()
  linkLibraries(ShArc.exe
                ShArc
                ArcGeoSim::ArcGeoSim_appli
                ArcGeoSim::ArcGeoSim_utils
                ArcGeoSim::ArcGeoSim_mesh
                ArcGeoSim::ArcGeoSim_numerics
                ArcGeoSim::ArcGeoSim_time
                ArcGeoSim::ArcGeoSim_physics
                ArcGeoSim::ArcGeoSim_tests
               )
endif()

if(TARGET alien)
  linkLibraries(ShArc.exe alien)
endif()
if(TARGET intel)
  linkLibraries(ShArc.exe intel)
endif()

commit(ShArc.exe)
```

L'exécutable est donc construit automatiquement lors du build global :

```bash
cmake -S sharc -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR}
cmake --build build-sharc
```

## Utilisation

Le `main()` est minimal :

```cpp
int main(int argc, char* argv[])
{ 
  Arcane::ApplicationInfo info(&argc,&argv,
                               "ShArc",
                               Arcane::VersionInfo(1,0,0));
  
  return ArcGeoSim::main(info);
}
```

Pour lancer une simulation, il suffit d'exécuter `ShArc.exe` avec un fichier de cas `.arc` approprié :

```bash
./ShArc.exe use-case-two-phase-flow.arc
```

Le fichier `.arc` référence les modules à charger (par exemple `ShArc`, `TwoPhaseFlowSimulation`, `ThermoChemicalConvection`) et les services associés.

*** Add File: src/ExaDiBench/README.md
# ExaDiBench

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source et des cas de test associés

## Description

Le module `ExaDiBench` fournit une **batterie de benchmarks géométriques** pour tester et valider les fonctionnalités de maillage et de géométrie d'ArcGeoSim/Arcane. Il s'agit principalement de :

- déformer un maillage (déplacement aléatoire contrôlé des nœuds intérieurs),
- recalculer les grandeurs géométriques (volumes, surfaces, centres),
- vérifier la cohérence des groupes de bord (`OuterFaces`, nœuds de bord),
- évaluer l'impact des déformations sur les mesures globales (volume total, surface totale, centre global).

Les cas de test correspondants se trouvent dans `test/ExaDiBench/` (cube 3D, géométrie SPE11c, anneaux, etc.).

## Fonctionnalités

Basé sur `ExaDiBenchModule.cc` et le descripteur `.axl` :

- ✅ Export de maillage dans différents formats (optionnel) :
  - VTK (`VtkLegacyMeshWriter`),
  - MSH (`MshNewMeshWriter`),
  - IXM (`IXMMeshWriter`).

- ✅ Construction des groupes de bord :
  - Construction d'un groupe de nœuds de bord (`BorderNodes`) et de faces de bord (`BorderFaces`).
  - Vérification de la cohérence avec le groupe `OuterFaces` de la famille de faces.

- ✅ Gestion de la géométrie :
  - Initialisation du service `IGeometryMng`.
  - Ajout de propriétés géométriques : volumes, surfaces, centres pour les mailles et faces de bord.
  - Mise à jour de la géométrie avec une politique `NoOptimizationGeometryPolicy`.

- ✅ Benchmark de déformation :
  - Sélection des nœuds intérieurs (non de bord).
  - Déplacement aléatoire des coordonnées (rayon contrôlé par `factor`).
  - Synchronisation des coordonnées et mise à jour géométrique.
  - Re-calcul et affichage des mesures globales (volume, surface, centre).

- ✅ Mesures et vérifications :
  - Calcul du volume total via `volumeCompute()` et vérification contre `IGeometry::computeMeasure()` cellule par cellule.
  - Calcul de la surface totale de bord via `surfaceCompute()`.
  - Calcul du centre global via `centerCompute()`.

## Prérequis

- Arcane / ArcGeoSim :
  - `IGeometryMng`, `IGeometry`, `PolyhedralGeometry`.
  - Gestion des groupes (`ItemGroupBuilder`).
  - Services de sortie maillage (`IMeshWriter`).

- Fichiers de configuration et maillages :
  - `.arc` de benchmark (voir `test/ExaDiBench/bench-*.arc`).
  - Maillages `.vt2` (ex. `cube3D1.vt2`).

## Installation

Le module `ExaDiBench` est déclaré dans `src/ExaDiBench/config.xml` et intégré dans la bibliothèque ShArc par la configuration CMake globale. Il n'y a pas de CMakeLists dédié dans ce répertoire.

## Utilisation

Le cycle principal est :

1. **init()**
   - Export optionnel du maillage en VTK/MSH/IXM selon les options (`exportVtk`, `exportMsh`, `exportIxm`).
   - Affinage du maillage si `refineLevel > 0` (fonctionnalité marquée "not yet implemented").
   - Construction des groupes de bord (nœuds/faces).
   - Initialisation de `IGeometryMng` et ajout des propriétés géométriques.
   - Mise à jour de la géométrie et calcul des mesures initiales (volume, surface, centre).

2. **compute()**
   - Déformation des nœuds intérieurs par un bruit pseudo‑aléatoire 3D.
   - Synchronisation des coordonnées.
   - Mise à jour de la géométrie.
   - Recalcul et affichage des mesures globales.

3. (optionnel) **restore()** / **refineMesh()**
   - Points d'extension prévus pour restaurer un état de référence ou affiner le maillage.

Les fichiers `.arc` de `test/ExaDiBench/` sélectionnent le maillage, le niveau de raffinement, le facteur de perturbation des nœuds, et les éventuels exports.

## API / Interface

### Classe principale

| Symbole | Type | Description |
|---------|------|-------------|
| `ExaDiBenchModule` | classe | Module Arcane de benchmark géométrique. Pilote export, déformation et mesures globales du maillage. |

### Méthodes clés (extrait)

| Méthode | Rôle |
|---------|------|
| `void init()` | Initialise l'état du benchmark (geometry, groupes, mesures initiales, export éventuel). |
| `void compute()` | Applique la déformation sur les nœuds intérieurs, met à jour la géométrie et recalcule les mesures globales. |
| `void refineMesh(Integer level)` | Point d'extension pour raffiner le maillage (actuellement non implémenté, message d'avertissement). |
| `Real volumeCompute()` | Calcule et vérifie le volume total du maillage sur les mailles propres. |
| `Real surfaceCompute()` | Calcule la surface totale des faces de bord. |
| `Real3 centerCompute()` | Calcule le centre de masse géométrique du maillage. |

## Architecture

```text
┌────────────────────────────┐
│        src/ExaDiBench      │
├────────────────────────────┤
│ ExaDiBenchModule.cc/.h     │
│ ExaDiBench.axl             │
│ MeshbMeshReaderService.cc  │
│ config.xml                 │
└────────────────────────────┘
          │          │
          ▼          ▼
     IGeometryMng   Cas .arc
     (géométrie)    (test/ExaDiBench)
```

`MeshbMeshReaderService.cc` est actuellement un service de lecture Meshb minimal/stub, principalement utilisé pour des tests ou extensions futures.

## Dépendances

| Dépendance | Usage |
|------------|-------|
| `IGeometryMng`, `IGeometry` | Gestion et calcul des propriétés géométriques. |
| `IMeshWriter` | Export VTK/MSH/IXM du maillage. |
| `ItemGroupBuilder` | Construction des groupes de nœuds/faces de bord. |
| Arcane ParallelMng | Réductions globales (somme des volumes, surfaces, centres). |

## Ressources

- `src/ExaDiBench/ExaDiBench.axl` — descripteur du module (points d'entrée, options, services).
- `test/ExaDiBench/*.arc` — cas de benchmark (cube 3D, SPE11c, anneaux, etc.).

*** Add File: test/TwoPhaseFlowSimulation/README.md
# Tests TwoPhaseFlowSimulation

> 🧠 Documentation enrichie par IA — basée sur l'analyse de la nomenclature des cas `.arc` et des maillages

## Description

Le répertoire `test/TwoPhaseFlowSimulation/` regroupe les **cas de test de référence** pour le module `TwoPhaseFlowSimulation`.
On y trouve à la fois :

- des cas synthétiques simples (maillage 10×1×1, puits),
- des cas de type **SPE10** (maillage hétérogène de grande taille),
- des variantes explorant différents solveurs et préconditionneurs linéaires (Hypre, PETSc, Trilinos/IFPS, MCGS, GPU/SYCL, etc.),
- des variantes physiques (avec/ sans pression capillaire, avec/ sans puits).

## Contenu

### Maillages et configuration

- `mesh-10x1x1.vt2` — maillage simple 10×1×1 (cas de base). 
- `mesh-well.vt2` — maillage avec puits.
- `SPE10-mesh-level0.vt2` — maillage SPE10 niveau 0.
- `SPE10-mesh-level0-well.vt2` — maillage SPE10 avec puits.
- `ShArc.config` — configuration ShArc/Arcane pour ces cas.

### Cas `.arc` principaux

- **Cas de base** :
  - `use-case-two-phase-flow.arc` — cas standard deux phases sur un maillage simple.
  - `use-case-two-phase-flow-with-capillary-pressure.arc` — même cas avec pression capillaire activée.
  - `use-case-two-phase-flow-well.arc` — cas deux phases avec puits.

- **Cas SPE10 (maillage hétérogène)** :
  - `use-case-two-phase-flow-spe10.arc` — configuration de base SPE10.
  - `use-case-two-phase-flow-spe10-well.arc` — SPE10 avec puits.

- **Variantes solveurs / préconditionneurs** (SPE10) :
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
  - Solutions "classiques" BCGS :
    - `use-case-two-phase-flow-spe10-bcgs-ilu0.arc`
    - `use-case-two-phase-flow-spe10-bcgs-cxr.arc`
    - `use-case-two-phase-flow-spe10-bcgs-cxr-sycl.arc`
  - TRI (Trilinos) :
    - `use-case-two-phase-flow-spe10-tri-bcgs-ilu0.arc`

### Fichiers de référence

- `RefUseCaseTwoPhaseFlow_DebugNewtonMatrix_1_1.txt` — référence pour la matrice du système non linéaire (diagnostic Newton).
- `RefUseCaseTwoPhaseFlow_DebugNewtonVector_1_1.txt` — référence pour le vecteur résidu.

## Objectifs des tests

- **Validation physique** :
  - Vérifier la cohérence des résultats (pression, saturations) sur des maillages simples et SPE10.
  - Tester l'effet des puits et de la pression capillaire.

- **Comparaison de solveurs** :
  - Comparer la robustesse et les performances de différentes combinaisons solveur/préconditionneur (Hypre, PETSc, IFPS, MCGS, HTS, GPU/SYCL, etc.).

- **Régression numérique** :
  - Utiliser les fichiers de référence Newton pour détecter les régressions dans l’assemblage du système.

## Utilisation

Depuis le répertoire de build :

```bash
ctest -R TwoPhaseFlowSimulation
```

Ou directement :

```bash
./ShArc.exe use-case-two-phase-flow-spe10-petsc-bcgs-ilu0.arc
```

Adapter le nombre de processus MPI et les options en fonction de la configuration CMake.

*** Add File: test/ExaDiBench/README.md
# Tests ExaDiBench

> 🧠 Documentation enrichie par IA — basée sur la liste des cas `.arc` et le module ExaDiBench

## Description

Le répertoire `test/ExaDiBench/` contient les cas de benchmark pour le module `ExaDiBench`. Ils exercent la chaîne :

- lecture et initialisation du maillage (cube 3D, SPE11c, anneaux),
- configuration de la géométrie (volumes, surfaces, centres),
- déformation des nœuds intérieurs,
- calcul et comparaison des mesures globales (volume, surface, centre).

## Contenu

### Maillages

- `cube3D1.vt2` — maillage de référence pour le cas "cube 3D".

### Cas `.arc`

- `bench-cube3d.arc` — benchmark de déformation sur le cube 3D (cas principal utilisé par la suite de tests).
- `bench-pb.arc` / `bench-pb-msh.arc` — cas "pb" (problème générique) sur différents formats de maillage (VTK/MSH).
- `bench-ring.arc` / `bench-ring-msh.arc` — benchmarks sur géométrie en anneau.
- `bench-spe11c.arc` / `bench-spe11c-structured.arc` — cas basés sur le cas industriel SPE11c (versions structurée et non structurée).
- `spe11c_structured.arc` — configuration SPE11c structurée.

### Configuration ShArc

- `ShArc.config` — configuration ShArc/Arcane pour ces cas.

## Objectifs des tests

- Vérifier la robustesse du calcul géométrique (volumes/surfaces/centres) sous perturbation.
- Tester la cohérence des groupes de bord (`OuterFaces`) sur des maillages variés.
- Garantir que les exports de maillage (VTK/MSH/IXM) restent fonctionnels.

## Utilisation

Depuis le répertoire de build :

```bash
ctest -R ExaDiBench
```

Pour lancer un cas particulier :

```bash
./ShArc.exe bench-cube3d.arc
```

*** Add File: test/ThermoChemicalConvection/README.md
# Tests ThermoChemicalConvection

> 🧠 Documentation enrichie par IA — basée sur l'analyse des fichiers de cas et du module ThermoChemicalConvection

## Description

Le répertoire `test/ThermoChemicalConvection/` contient un cas de test pour le module `ThermoChemicalConvection`, qui résout un système couplé pression–concentration–température en milieu poreux.

Le cas met en jeu :

- un maillage 3D `mesh-10x1x10.vt2`,
- un fichier de cas `use-case-thermal.arc` configurant le système ArcRes et les services (conditions initiales, lois, conditions aux limites),
- un fichier `ShArc.config` pour l'infrastructure ShArc/Arcane.

## Objectifs

- Valider la construction de l'accumulation et des flux (advection, diffusion, conduction) définis dans `ThermoChemicalConvectionModule`.
- Vérifier la cohérence thermique (capacité et conductivité des phases fluide et solide) sur un cas simple.

## Utilisation

Depuis le répertoire de build :

```bash
ctest -R ThermoChemicalConvection
```

Ou directement :

```bash
./ShArc.exe use-case-thermal.arc
```

*** Add File: test/CAWF/README.md
# Tests CAWF

> 🧠 Documentation enrichie par IA — basée sur le README CAWF et la structure des tests

## Description

Le répertoire `test/CAWF/` regroupe les cas de test pour le module CAWF (solveurs FEM Poisson, Elasticity, Compaction, DynamicMeshMng). Les cas détaillés (fichiers `.arc`, maillages, configurations) sont organisés dans des sous‑répertoires (par exemple `Compaction/`, `MecaFEM/`, `Poisson/`), en cohérence avec la structure décrite dans `src/CAWF/README.md`.

Ce dossier contient également des fichiers de configuration pour le couplage éventuel avec preCICE (`precice-config-*.xml`).

## Contenu

- `precice-config-v3.xml` / `precice-config-iter-v3.xml` — configurations preCICE pour des scénarios de couplage (itératif ou non).
- Sous‑répertoires contenant :
  - des maillages VTU/VTK/VT2,
  - des fichiers `.arc` pour les solveurs Poisson/Elasticity/Compaction,
  - des fichiers `ShArc.config` spécifiques.

## Objectifs des tests

- Vérifier les solveurs FEM sur des cas unitaires et multi‑physiques (Poisson, élasticité, compaction).
- Tester les différents backends linéaires (Hypre, PETSc, etc.) tels que configurés dans les `.arc`.
- Valider (le cas échéant) l'intégration avec preCICE pour les cas couplés.

## Utilisation

Depuis le répertoire de build :

```bash
ctest -R CAWF
```

Ou exécuter directement l'un des cas dans les sous‑répertoires, par exemple :

```bash
./ShArc.exe Poisson/use-case-poisson.arc
```
