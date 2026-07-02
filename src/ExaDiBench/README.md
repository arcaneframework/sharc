# ExaDiBench

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source

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

Le module `ExaDiBench` fournit un banc de test géométrique et maillage pour ShArc / ArcGeoSim. Il est utilisé pour
des scénarios de benchmark (ExaDiBench) qui mesurent notamment :

- la robustesse de l’infrastructure de maillage Arcane/ArcGeoSim,
- la précision et la cohérence des propriétés géométriques (volume, surface, centre),
- le comportement de l’export de maillage vers différents formats (VTK legacy, MSH, IXM),
- la sensibilité des quantités géométriques à de petites perturbations aléatoires des coordonnées.



  - VTK legacy (`VtkLegacyMeshWriter`) si `exportVtk` est activé.
  - Gmsh `.msh` (`MshNewMeshWriter`) si `exportMsh` est activé.
  - IXM (`IXMMeshWriter`) si `exportIxm` est activé.

  - Construction de `BorderNodes` et `BorderFaces` à partir des faces de domaine.
  - Vérification que le groupe `OuterFaces` correspond au groupe des faces de bord calculées.

  - Volume des cellules (`IGeometryProperty::PVolume`).
  - Aire des faces de bord (`IGeometryProperty::PArea`).
  - Centres de cellules (`IGeometryProperty::PCenter`).
  - Possibilité d’utiliser un stockage externe pour ces propriétés.

  - Déplacement des noeuds internes par une perturbation aléatoire de norme contrôlée (`factor`).
  - Mise à jour de la géométrie après perturbation.

  - `volumeCompute()` : somme des volumes des cellules, avec comparaison à la mesure calculée par `IGeometry`.
  - `surfaceCompute()` : somme des aires des faces de bord, avec comparaison à `IGeometry`.
  - `centerCompute()` : moyenne des centres de cellules.
  - Affichage de l’amplitude locale (min/max) des volumes et aires.




  - Arcane (maillage, services, IMeshWriter, time loop, IParallelMng).
  - ArcGeoSim (IGeometryMng, IGeometry, PolyhedralGeometry, ItemGroupBuilder).
  ArcGeoSim / ArcaneInfra.











   - Exporte le maillage dans les formats demandés (`exportVtk`, `exportMsh`, `exportIxm`).
   - Applique un raffinement éventuel via `refineMesh(level)` (actuellement non implémenté).
   - Construit les groupes `BorderNodes` et `BorderFaces` à partir des faces de domaine.
   - Vérifie la cohérence du groupe `OuterFaces`.
   - Initialise `IGeometryMng` et `IGeometryPolicy` (`NoOptimizationGeometryPolicy`).
   - Ajoute les propriétés géométriques (volume, aire, centre) et les met à jour.
   - Calcule et affiche le volume, la surface et le centre globaux (après réduction parallèle).

   - Réinitialise le checker de noeuds de bord si nécessaire.
   - Perturbe les coordonnées des noeuds internes (non-bord) avec un déplacement aléatoire de norme contrôlée
     par `factor` :

```cpp
VariableNodeReal3& coords = PRIMARYMESH_CAST(mesh())->nodesCoordinates();
ENUMERATE_NODE(inode,ownNodes()) {
  if (not m_border_node_checker.hasKey(*inode)) {
    const Real r = drand48()*factor;
    const Real theta = twopi*drand48();
    const Real phi = twopi*drand48();
    coords[inode] += r * Real3(cos(theta), sin(theta)*cos(phi), sin(theta)*sin(phi));
  }
}
coords.synchronize();
options()->geometry()->update(m_geometry_policy);
```

   - Recalcule volume, surface et centre après perturbation.

   - Pour l’instant se contente de logguer un message "not yet implemented".

   - Fonction placeholder pour restaurer l’état d’un maillage (actuellement simple log message).
















