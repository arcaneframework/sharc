# Tests ExaDiBench

> 🧠 Documentation enrichie par IA — basée sur la liste des cas `.arc` et le module ExaDiBench

## 📋 Table des matières

- [Description](#description)
- [Contenu](#contenu)
- [Objectifs des tests](#objectifs-des-tests)
- [Utilisation](#utilisation)
- [Ressources](#ressources)

## Description

Le répertoire `test/ExaDiBench/` contient les cas de benchmark pour le module `ExaDiBench`. Ils exercent la chaîne :

- lecture et initialisation du maillage (cube 3D, SPE11c, anneaux),
- configuration de la géométrie (volumes, surfaces, centres),
- déformation des nœuds intérieurs,
- calcul et comparaison des mesures globales (volume, surface, centre).

## Contenu

### Maillages

- `cube3D1.vt2` — maillage de référence pour le cas « cube 3D ».
- `spe11c.vtk` — maillage/visualisation associée au cas SPE11c.
- Maillages supplémentaires référencés dans les `.arc` (via les blocs `<mesh>`).

### Cas `.arc`

- `bench-cube3d.arc` — benchmark de déformation sur le cube 3D (cas principal utilisé par la suite de tests).
- `bench-pb.arc` / `bench-pb-msh.arc` — cas « pb » (problème générique) sur différents formats de maillage (VTK/MSH).
- `bench-ring.arc` / `bench-ring-msh.arc` — benchmarks sur géométrie en anneau.
- `bench-spe11c.arc` / `bench-spe11c-structured.arc` — cas basés sur le cas industriel SPE11c (versions structurée et non structurée).
- `spe11c_structured.arc` — configuration SPE11c structurée.

### Configuration ShArc

- `ShArc.config` — configuration ShArc/Arcane pour ces cas (time-loop, modules, points d’entrée).

## Objectifs des tests

- Vérifier la robustesse du calcul géométrique (volumes/surfaces/centres) sous perturbation.
- Tester la cohérence des groupes de bord (`OuterFaces`) sur des maillages variés.
- Garantir que les exports de maillage (VTK/MSH/IXM) restent fonctionnels avec le module `ExaDiBench`.

## Utilisation

Depuis le répertoire de build :

```bash
# Lancer les tests ExaDiBench via CTest
ctest -R ExaDiBench
```

Pour lancer un cas particulier :

```bash
./ShArc.exe test/ExaDiBench/bench-cube3d.arc
```

Adapter MPI et les options de build selon votre configuration Arcane/ArcGeoSim.

## Ressources

- `src/ExaDiBench/README.md` — documentation du module de benchmark géométrique.
- `src/ExaDiBench/ExaDiBench.axl` — descripteur complet (variables, options, points d’entrée).
- Documentation Arcane / ArcGeoSim — géométrie (`IGeometryMng`, `IGeometry`) et export de maillage (`IMeshWriter`).
