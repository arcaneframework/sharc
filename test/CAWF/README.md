# Tests CAWF

> 🧠 Documentation enrichie par IA — basée sur l'analyse des cas `.arc` et des READMEs CAWF

## 📋 Table des matières

- [Description](#description)
- [Organisation](#organisation)
- [Poisson](#poisson)
- [MecaFEM](#mecafem)
- [Compaction](#compaction)
- [Objectifs des tests](#objectifs-des-tests)
- [Utilisation](#utilisation)
- [Ressources](#ressources)

## Description

Le répertoire `test/CAWF/` regroupe les **cas de test de référence** pour la bibliothèque CAWF (ArcaneFEM Workflows Library), décrite dans `src/CAWF/README.md`. Il couvre trois grandes familles de problèmes :

- **Poisson** — résolution de problèmes de type diffusion / conduction sur maillages 2D/3D.
- **MecaFEM** — problèmes d’élasticité linéaire en 2D/3D (barres, cubes) avec différentes conditions de chargement.
- **Compaction** — cas de compaction de cube, d’anneau et de bassin synthétique.

Chaque sous-dossier contient des fichiers `.arc` qui instancient les modules CAWF correspondants, les maillages et les paramètres physiques.

## Organisation

- `Poisson/` — cas Poisson 2D/3D (cercles, sphères, carré perforé).
- `MecaFEM/` — cas d’élasticité linéaire (barres 3D, cubes).
- `Compaction/` — cas de compaction (cube, ring, bassin synthétique).
- `precice-config-v3.xml`, `precice-config-iter-v3.xml` — configurations PreCICE (couplage) pour certains scénarios.

Chaque sous-dossier contient également un `ShArc.config` définissant la boucle de temps et les modules CAWF utilisés.

## Poisson

Répertoire : `test/CAWF/Poisson/`

Cas typiques :

- `circle.2D.arc`, `circle.2D.quad.arc`  
  Problèmes Poisson 2D sur cercle (triangles/quadrangles).

- `circle.neumann.2D.arc`, `circle.neumann.2D.quad.arc`  
  Variantes avec conditions de Neumann.

- `perforatedSquare.pointDirichlet.2D.arc`  
  Carré perforé avec conditions de Dirichlet ponctuelles.

- `sphere.3D.arc`, `sphere.3D.hexa.arc`  
  Problèmes Poisson sur sphère 3D (tétrahèdres/hexaèdres).

- `sphere.neumann.3D.arc`, `sphere.neumann.3D.hexa.arc`  
  Variantes avec Neumann en 3D.

Tous ces cas utilisent les modules CAWF Poisson décrits dans `src/CAWF/Poisson/Readme.md` : maillages, conditions aux limites (Dirichlet/Neumann), post‑traitement de la variable `U`.

## MecaFEM

Répertoire : `test/CAWF/MecaFEM/`

Cas typiques :

- Barres 3D sous chargement :

  - `bar.3D.Dirichlet.bodyForce.arc`  
    Barre en 3D avec conditions de Dirichlet et force volumique (poids propre).

  - `bar.3D.Dirichlet.traction.bodyForce.arc`  
    Variante avec traction aux extrémités en plus de la force volumique.

  - `bar.3D.cartesian.Dirichlet.bodyForce.arc`  
    Version cartésienne du cas de barre.

- Cubes :

  - `mecafem-cube.arc`, `mecafem-cube-hexa.arc`  
    Cas cube de base (maillage tétra/héxa).

  - `mecafem-cube-hexa-hypre.arc`, `mecafem-cube-hexa-hypre-acc.arc`  
    Variantes utilisant Hypre (et options d’accélération) pour la résolution linéaire.

  - `mecafem-cube-cawf.arc`, `mecafem-cube-cawf-evol.arc`  
    Cas cubes utilisant la configuration CAWF, dont une variante évolutive.

Ces cas s’appuient sur la formulation décrite dans `src/CAWF/Elasticity/Readme.md` : matériaux élastiques linéaires, conditions de bord, calcul des déplacements.

## Compaction

Répertoire : `test/CAWF/Compaction/`

Cas typiques :

- `Compaction-cube.arc`  
  Cas cube simple de compaction.

- `Compaction-cube-cawf.arc`, `Compaction-cube-cawf-evol.arc`  
  Versions utilisant la configuration CAWF, avec une variante évolutive (`-evol`).

- `Compaction-ring.arc`  
  Cas de compaction sur géométrie annulaire.

- `Compaction-synth-basin.arc`  
  Cas de bassin synthétique (compaction de bassin).

Un log d’exécution (`Compaction-cube-cawf-evol.log`) donne un exemple de sortie pour le cas évolutif.

## Objectifs des tests

- **Validation numérique des solveurs CAWF** :
  - Vérifier la convergence et la qualité des solutions (champ `U` pour Poisson/Elasticity, observables de compaction).
  - Valider la robustesse sur différentes topologies de maillage (2D/3D, tri/hexa/quads).

- **Validation du couplage et des options backend** :
  - Tester les solveurs linéaires (Hypre, backends Acc, etc.) dans des cas mécaniciens/compaction réalistes.
  - Utiliser, le cas échéant, les configurations PreCICE pour des scénarios couplés.

- **Régression fonctionnelle** :
  - Servir de base de non‑regression pour les modifications dans `src/CAWF/` (FEMUtils, solveurs Poisson/Elasticity/Compaction).

## Utilisation

Depuis le répertoire de build :

```bash
# Lancer les tests CAWF via CTest
ctest -R CAWF
```

Pour lancer un cas particulier :

```bash
# Exemple : cas Poisson cercle 2D
./ShArc.exe test/CAWF/Poisson/circle.2D.arc

# Exemple : cas d'élasticité barre 3D
./ShArc.exe test/CAWF/MecaFEM/bar.3D.Dirichlet.bodyForce.arc

# Exemple : cas de compaction cube
./ShArc.exe test/CAWF/Compaction/Compaction-cube-cawf.arc
```

Adapter les options (MPI, solveurs) en fonction de votre configuration Arcane/CAWF.

## Ressources

- `src/CAWF/README.md` — documentation globale de CAWF.
- `src/CAWF/Poisson/Readme.md` — formulation et configuration des cas Poisson.
- `src/CAWF/Elasticity/Readme.md` — formulation et configuration des cas d’élasticité.
- `docs/content/test/CAWF/` (si présent) — documentation Hugo associée aux tests CAWF.
