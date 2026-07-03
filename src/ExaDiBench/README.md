# ExaDiBench

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source et des cas de test associés

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

Le module `ExaDiBench` fournit une **batterie de benchmarks géométriques** pour tester et valider les fonctionnalités de maillage et de géométrie d'ArcGeoSim/Arcane. Il s'agit principalement de :

- déformer un maillage (déplacement aléatoire contrôlé des nœuds intérieurs),
- recalculer les grandeurs géométriques (volumes, surfaces, centres),
- vérifier la cohérence des groupes de bord (`OuterFaces`, groupes de nœuds/faces de bord),
- évaluer l'impact des déformations sur des mesures globales (volume total, surface totale, centre global).

Les cas de test correspondants se trouvent dans `test/ExaDiBench/` (cube 3D, géométrie SPE11c, anneaux, etc.).

Le module s'appuie sur le service `IGeometryMng` pour gérer les propriétés géométriques et sur les services de sortie `IMeshWriter` pour exporter le maillage dans différents formats.

## Fonctionnalités

À partir de `ExaDiBenchModule.cc` et `ExaDiBench.axl` :

- ✅ **Export de maillage** (optionnel) :
  - VTK (`VtkLegacyMeshWriter`),
  - MSH (`MshNewMeshWriter`),
  - IXM (`IXMMeshWriter`),
  - noms de fichiers dérivés de `mesh()->name()` avec suffixes `-vtk`, `-msh`, `-ixm`.

- ✅ **Construction des groupes de bord** :
  - Construction d'un groupe de nœuds de bord (`BorderNodes`) et de faces de bord (`BorderFaces`) à partir des faces physiques de bord (`face.isSubDomainBoundary()`).
  - Vérification de la cohérence avec le groupe `OuterFaces` :
    - sur versions anciennes d’Arcane : `mesh()->faceFamily()->findGroup("OuterFaces").own()`,
    - sur versions récentes : `outerFaces().own()`.
  - Check strict : taille de `OuterFaces` == taille du groupe reconstruit, sinon `fatal()`.

- ✅ **Gestion de la géométrie** :
  - Initialisation du service `IGeometryMng` via l’option `geometry`.
  - Création d’une politique `NoOptimizationGeometryPolicy`.
  - Enregistrement des propriétés géométriques :
    - `PArea` pour toutes les faces,
    - `PCenter` et `PVolume` pour les mailles (via groupe `m_cell_group`).
  - Support de **stockage externe** (`use-external-storage`) dans des variables explicites :
    - `TestVolumes`, `TestSurfaces`, `TestCenters`.

- ✅ **Benchmark de déformation** :
  - Sélection des nœuds intérieurs (n’appartenant pas au groupe `BorderNodes`).
  - Perturbation pseudo‑aléatoire 3D des coordonnées des nœuds intérieurs :
    - rayon `r` contrôlé par l’option `factor`,
    - angles aléatoires `theta`, `phi` (2π * aléa).
  - Synchronisation des coordonnées (`coords.synchronize()`).
  - Mise à jour de la géométrie avec la politique définie.
  - Re-calcul des mesures globales et réduction MPI (`ReduceSum`).

- ✅ **Mesures et vérifications** :
  - `volumeCompute()` :
    - somme des volumes sur `ownCells()`,
    - comparaison de chaque volume stocké avec `geom->computeMeasure(*icell)` (fatal en cas d’écart significatif),
    - trace de l’amplitude des volumes locaux (min/max).
  - `surfaceCompute()` :
    - somme de `PArea` sur les faces de bord `m_border_face_group`.
  - `centerCompute()` :
    - somme pondérée des centres de chaque cellule, normalisation via réduction MPI.

- ⚠️ **Fonctions d’extension** :
  - `refineMesh(Integer level)` : stub (affiche un message *not yet implemented*).
  - `restore()` : stub (traces uniquement).

## Prérequis

- **Langage / frameworks** :
  - C++ avec Arcane / ArcGeoSim (modules, services, maillage, géométrie).
  - AXL (`ExaDiBench.axl`) pour décrire variables, options et points d’entrée.

- **Services utilisés** :
  - `IGeometryMng` (service de géométrie),
  - `IMeshWriter` (services de sortie maillage : VTK, MSH, IXM),
  - `IParallelMng` (réductions globales),
  - `ItemGroupBuilder` (construction de groupes dynamiques de nœuds/faces).

- **Build** :
  - Le module est déclaré dans `src/ExaDiBench/config.xml` :
    ```xml
    <makefile>
      <needed-packages>
        <package name="arcane" />
      </needed-packages>

      <files language="module">
        <file>ExaDiBench</file>
      </files>

      <files language="c++">
        <file header='false'>MeshbMeshReaderService</file>
      </files>
    </makefile>
    ```
  - Il est intégré à la bibliothèque `ShArc` via `src/CMakeLists.txt`.

## Installation

Aucune étape dédiée pour `ExaDiBench` : il est compilé automatiquement lorsque la bibliothèque ShArc est construite.

Rappel de la séquence globale (voir README racine pour les détails) :

```bash
cmake -S sharc -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR} \
  -DUSE_ARCANE_V3=ON

cmake --build build-sharc
```

L’exécutable `ShArc.exe` expose ensuite le module `ExaDiBench` utilisable dans les fichiers de cas `.arc`.

## Utilisation

### Cycle de vie du module

Les points d’entrée sont définis dans `ExaDiBench.axl` :

```xml
<entry-points>
  <entry-point method-name="init"    name="Init"    where="init"         property="none" />
  <entry-point method-name="compute" name="Compute" where="compute-loop" property="none" />
  <entry-point method-name="restore" name="Restore" where="restore"      property="none" />
</entry-points>
```

Côté C++ :

- `init()` :
  - export du maillage selon `export-vtk`, `export-msh`, `export-ixm`,
  - appel éventuel à `refineMesh(refine-level)` (actuellement stub),
  - construction des groupes de bord (`BorderNodes`, `BorderFaces`),
  - initialisation de `IGeometryMng` et enregistrement des propriétés,
  - calcul des mesures globales initiales (volume, surface, centre) + réductions MPI,
  - initialisation de `m_localId` pour diagnostic.

- `compute()` :
  - re‑validation du checker de nœuds de bord,
  - perturbation pseudo‑aléatoire des nœuds intérieurs (amplitude `factor`),
  - synchronisation des coordonnées,
  - mise à jour de la géométrie,
  - recalcul des mesures globales (volume, surface, centre) et affichage.

### Exemple d’exécution

Depuis le répertoire de build, avec les cas de test :

```bash
# Lancer la suite de tests ExaDiBench via CTest
ctest -R ExaDiBench

# Lancer un cas particulier
./ShArc.exe test/ExaDiBench/bench-cube3d.arc
```

Les options (export, niveau de raffinement, facteur de perturbation) se configurent dans le fichier `.arc` via le bloc d’options du module `ExaDiBench`.

## API / Interface

### Classe principale

| Symbole            | Type   | Description |
|--------------------|--------|-------------|
| `ExaDiBenchModule` | classe | Module Arcane de benchmark géométrique. Pilote export, déformation et mesures globales du maillage. |

### Méthodes clés (extrait)

| Méthode                                | Type / rôle |
|----------------------------------------|------------|
| `void init()`                          | Point d’entrée `Init`. Initialise le benchmark : export éventuel, groupes de bord, géométrie, mesures initiales. |
| `void compute()`                       | Point d’entrée `Compute`. Applique la perturbation aux nœuds intérieurs, met à jour la géométrie et recalcule les mesures globales. |
| `void refineMesh(Integer level)`       | Helper pour le raffinement du maillage (actuellement non implémenté, message d’avertissement). |
| `void restore()`                       | Point d’entrée `Restore` (stub, trace un message). |
| `Real volumeCompute()`                 | Calcule le volume total et vérifie la cohérence avec `IGeometry::computeMeasure()`. |
| `Real surfaceCompute()`                | Calcule la surface totale sur les faces de bord. |
| `Real3 centerCompute()`                | Calcule le centre de masse géométrique global du maillage. |

### Variables géométriques (ExaDiBench.axl)

| Nom logique      | Champ interne | Type   | Support | Description |
|------------------|--------------|--------|--------|-------------|
| `TestVolumes`    | `volumes`    | real   | cell   | Volume par maille (stockage optionnel de `PVolume`). |
| `TestCenters`    | `centers`    | real3  | cell   | Centre par maille (stockage optionnel de `PCenter`). |
| `TestSurfaces`   | `surfaces`   | real   | face   | Surface par face de bord (stockage optionnel de `PArea`). |
| `LocalId`        | `localId`    | integer| cell   | Identifiant local de la maille (diagnostic). |

## Architecture

```text
┌────────────────────────────────────┐
│          src/ExaDiBench           │
├────────────────────────────────────┤
│ ExaDiBenchModule.cc / .h          │
│ ExaDiBench.axl                    │
│ MeshbMeshReaderService.cc         │
│ config.xml                        │
└────────────────────────────────────┘
          │                │
          ▼                ▼
   IGeometryMng        Cas .arc
   (géométrie)        (test/ExaDiBench)

```

- `config.xml` déclare le module et le service `MeshbMeshReaderService`.
- `ExaDiBench.axl` définit les variables, options et points d’entrée.
- `ExaDiBenchModule` implémente la logique de benchmark.
- `MeshbMeshReaderService` fournit un point d’extension pour la lecture de maillages Meshb (implémentation actuelle minimale).

## Dépendances

| Dépendance         | Usage |
|--------------------|-------|
| `IGeometryMng`     | Gestion des propriétés géométriques (volumes, surfaces, centres) et mise à jour. |
| `IGeometry`        | Calcul de mesures géométriques de référence (`computeMeasure`). |
| `IMeshWriter`      | Export du maillage en VTK, MSH et IXM. |
| `ItemGroupBuilder` | Construction dynamique de groupes de nœuds/faces de bord. |
| `IParallelMng`     | Réductions globales MPI (somme de volumes, surfaces, centres). |
| Arcane / ArcGeoSim | Infrastructure de module, maillage, parallélisme. |

## Configuration

Les principales options sont définies dans `ExaDiBench.axl` :

| Option                | Type    | Défaut   | Description |
|-----------------------|---------|----------|-------------|
| `export-vtk`          | bool    | `false`  | Export du maillage au format VTK. |
| `export-msh`          | bool    | `false`  | Export MSH. |
| `export-ixm`          | bool    | `false`  | Export IXM. |
| `refine-level`        | integer | `0`      | Niveau de raffinement du maillage (fonctionnalité à implémenter). |
| `factor`              | real    | `0.5`    | Amplitude de perturbation géométrique des nœuds intérieurs. |
| `use-external-storage`| bool    | `false`  | Stocker mesures géométriques dans les variables explicites (`TestVolumes`, etc.) plutôt que dans le stockage interne. |
| `geometry`            | service | (à fournir) | Instance de service `IGeometryMng` à utiliser. |

Exemple d’extrait `.arc` (schématique) :

```xml
<module name="ExaDiBench">
  <options>
    <export-vtk>true</export-vtk>
    <factor>0.5</factor>
    <use-external-storage>false</use-external-storage>
    <geometry>GeometryService</geometry>
  </options>
</module>
```

## Ressources

- `src/ExaDiBench/ExaDiBench.axl` — descripteur complet du module.
- `test/ExaDiBench/*.arc` — cas de benchmark (cube 3D, SPE11c, anneaux, etc.).
- Documentation Arcane / ArcGeoSim — sections sur :
  - la géométrie (`IGeometryMng`, `IGeometry`, propriétés d’éléments),
  - les services d’export de maillage (`IMeshWriter`),
  - les groupes d’items (`ItemGroup`, `ItemGroupBuilder`).
- `docs/content/test/ExaDiBench/` (si présent) — documentation Hugo associée aux tests ExaDiBench.
