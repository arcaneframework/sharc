#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Génération de maillages de la sphère unité avec Gmsh (API Python),
en tétraèdres et en hexaèdres, pour plusieurs tailles cibles,
export au format .msh binaire.

Cibles demandées : 28 000 / 212 000 / 1 000 000 / 24 000 000 mailles

Principe :
----------
- TÉTRAÈDRES : sphère OCC pleine + champ de taille uniforme "h", ajusté par
  quelques itérations (loi h ~ (V / N)^(1/3)) pour converger vers le nombre
  de mailles cible.

- HEXAÈDRES : la sphère est topologiquement découpée en 7 blocs structurés
  transfinis, technique classique dite "cube en sphère" (O-grid) :
      * 1 cube central (hexa réguliers)
      * 6 blocs extérieurs courbes reliant chaque face du cube à la portion
        de sphère correspondante (arcs de grand cercle), chacun maillé en
        n x n x n hexaèdres, puis "recombine" pour forcer les hexas.
  Un maillage 100% hexaédrique d'une sphère est nécessairement "structuré" :
  le nombre de mailles obtenu est ~ 7*n^3 et ne peut donc être ajusté que
  par paliers (on choisit le n le plus proche de la cible, écart typique
  < 3 %).

Installation requise :
    pip install gmsh

Utilisation :
    python3 sphere_meshes.py
Les fichiers sont écrits dans ./meshes/
"""

import gmsh
import math
import os

# ---------------------------------------------------------------------
# Paramètres généraux
# ---------------------------------------------------------------------

TARGETS = [28_000, 212_000, 1_000_000, 24_000_000]
RADIUS = 1.0
OUTDIR = "meshes"

MSH_VERSION = 4.1   # 2.2 = compatible avec la quasi-totalité des solveurs
                    # (mettre 4.1 pour le format Gmsh le plus récent)


# ---------------------------------------------------------------------
# Utilitaires communs
# ---------------------------------------------------------------------

def init_gmsh():
    gmsh.initialize()
    gmsh.option.setNumber("General.Terminal", 1)
    gmsh.option.setNumber("General.Verbosity", 2)
    gmsh.option.setNumber("Mesh.Binary", 1)          # écriture binaire
    gmsh.option.setNumber("Mesh.MshFileVersion", MSH_VERSION)


def write_mesh(filename):
    os.makedirs(OUTDIR, exist_ok=True)
    path = os.path.join(OUTDIR, filename)
    gmsh.write(path)
    print(f"    -> écrit : {path}")
    return path


def count_elements(dim):
    etypes, etags, _ = gmsh.model.mesh.getElements(dim)
    return sum(len(t) for t in etags)


# ---------------------------------------------------------------------
# 1) Maillage tétraédrique de la sphère pleine
# ---------------------------------------------------------------------

def build_tet_sphere(n_target, filename, max_iter=6, tol=0.02):
    """Sphère pleine (OCC), maillage tétraédrique de taille quasi uniforme,
    ajustée itérativement pour approcher n_target éléments."""

    gmsh.model.add(f"sphere_tet_{n_target}")
    gmsh.model.occ.addSphere(0, 0, 0, RADIUS)
    gmsh.model.occ.synchronize()

    gmsh.option.setNumber("Mesh.MeshSizeFromCurvature", 0)
    gmsh.option.setNumber("Mesh.MeshSizeExtendFromBoundary", 1)
    gmsh.option.setNumber("Mesh.Algorithm3D", 10)   # HXT : rapide, adapté aux gros volumes

    V_sphere = 4.0 / 3.0 * math.pi * RADIUS ** 3
    # volume moyen d'un tétraèdre "régulier" d'arête h : h^3/(6*sqrt(2))
    h = (V_sphere * 6.0 * math.sqrt(2.0) / n_target) ** (1.0 / 3.0)

    ntet = None
    for it in range(max_iter):
        gmsh.option.setNumber("Mesh.MeshSizeMin", h)
        gmsh.option.setNumber("Mesh.MeshSizeMax", h)
        gmsh.model.mesh.clear()
        gmsh.model.mesh.generate(3)
        ntet = count_elements(3)
        ratio = ntet / n_target
        print(f"    [tet] it{it}: h={h:.5f} -> {ntet} tets "
              f"(cible {n_target}, écart {(ratio-1)*100:+.1f}%)")
        if abs(ratio - 1.0) < tol:
            break
        h *= ratio ** (1.0 / 3.0)

    write_mesh(filename)
    return ntet


# ---------------------------------------------------------------------
# 2) Maillage hexaédrique de la sphère pleine (technique "cube en sphère")
# ---------------------------------------------------------------------

def build_hex_sphere(n_target, filename, cube_ratio=0.55):
    """
    cube_ratio in (0,1) : taille du cube central, en fraction du rayon
    inscrit R/sqrt(3). Une valeur ~0.5-0.6 donne un bon compromis de
    qualité (angle/écrasement) sur les 6 blocs extérieurs.
    """

    n = max(1, round((n_target / 7.0) ** (1.0 / 3.0)))
    print(f"    [hex] n={n} subdivisions/arête -> ~{7*n**3} hexas "
          f"(cible {n_target})")

    gmsh.model.add(f"sphere_hex_{n_target}")
    geo = gmsh.model.geo

    R = RADIUS
    c = (R / math.sqrt(3.0)) * cube_ratio  # demi-côté du cube central

    # 8 sommets du cube central, et leurs projections radiales sur la sphère
    signs = [(-1, -1, -1), (1, -1, -1), (1, 1, -1), (-1, 1, -1),
             (-1, -1, 1), (1, -1, 1), (1, 1, 1), (-1, 1, 1)]
    d = math.sqrt(3.0)

    cube_pts = [geo.addPoint(sx * c, sy * c, sz * c) for sx, sy, sz in signs]
    sph_pts = [geo.addPoint(sx * R / d, sy * R / d, sz * R / d) for sx, sy, sz in signs]
    center = geo.addPoint(0, 0, 0)

    # arêtes du cube (12) et arcs de grand cercle correspondants (12)
    edges_idx = [(0, 1), (1, 2), (2, 3), (3, 0),
                 (4, 5), (5, 6), (6, 7), (7, 4),
                 (0, 4), (1, 5), (2, 6), (3, 7)]

    cube_lines, sph_arcs = {}, {}
    for (a, b) in edges_idx:
        l = geo.addLine(cube_pts[a], cube_pts[b])
        cube_lines[(a, b)], cube_lines[(b, a)] = l, -l
        arc = geo.addCircleArc(sph_pts[a], center, sph_pts[b])
        sph_arcs[(a, b)], sph_arcs[(b, a)] = arc, -arc
        geo.mesh.setTransfiniteCurve(l, n + 1)
        geo.mesh.setTransfiniteCurve(arc, n + 1)

    # 8 lignes radiales cube -> sphère
    radial = [geo.addLine(cube_pts[i], sph_pts[i]) for i in range(8)]
    for l in radial:
        geo.mesh.setTransfiniteCurve(l, n + 1)

    faces_idx = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4),
                 (1, 2, 6, 5), (2, 3, 7, 6), (3, 0, 4, 7)]

    def curve_loop(idxs, lines_dict):
        a, b, cc, dd = idxs
        return geo.addCurveLoop([lines_dict[(a, b)], lines_dict[(b, cc)],
                                  lines_dict[(cc, dd)], lines_dict[(dd, a)]])

    # --- cube central : 6 faces planes + volume transfini ---
    cube_faces = []
    for f in faces_idx:
        cl = curve_loop(f, cube_lines)
        s = geo.addPlaneSurface([cl])
        geo.mesh.setTransfiniteSurface(s)
        geo.mesh.setRecombine(2, s)
        cube_faces.append(s)

    cube_sl = geo.addSurfaceLoop(cube_faces)
    cube_vol = geo.addVolume([cube_sl])
    geo.mesh.setTransfiniteVolume(cube_vol, [cube_pts[i] for i in range(8)])

    # --- 6 blocs extérieurs : face du cube (réutilisée) -> patch sphérique ---
    outer_vols = []
    for fi, f in enumerate(faces_idx):
        a, b, cc, dd = f
        bottom = cube_faces[fi]  # surface partagée avec le cube central

        cl_top = geo.addCurveLoop([sph_arcs[(a, b)], sph_arcs[(b, cc)],
                                    sph_arcs[(cc, dd)], sph_arcs[(dd, a)]])
        top = geo.addSurfaceFilling([cl_top])
        geo.mesh.setTransfiniteSurface(top)
        geo.mesh.setRecombine(2, top)

        idxs4 = [a, b, cc, dd]
        side_surfs = []
        for k in range(4):
            i0, i1 = idxs4[k], idxs4[(k + 1) % 4]
            cl_side = geo.addCurveLoop([cube_lines[(i0, i1)], radial[i1],
                                         -sph_arcs[(i0, i1)], -radial[i0]])
            s_side = geo.addSurfaceFilling([cl_side])
            geo.mesh.setTransfiniteSurface(s_side)
            geo.mesh.setRecombine(2, s_side)
            side_surfs.append(s_side)

        sl = geo.addSurfaceLoop([-bottom, top] + side_surfs)
        vol = geo.addVolume([sl])
        geo.mesh.setTransfiniteVolume(
            vol, [cube_pts[a], cube_pts[b], cube_pts[cc], cube_pts[dd],
                  sph_pts[a], sph_pts[b], sph_pts[cc], sph_pts[dd]])
        outer_vols.append(vol)

    geo.synchronize()
    gmsh.model.addPhysicalGroup(3, [cube_vol] + outer_vols, name="sphere")

    gmsh.model.mesh.generate(3)
    nhex = count_elements(3)
    print(f"    [hex] maillage généré : {nhex} hexaèdres "
          f"(écart {(nhex/n_target-1)*100:+.1f}%)")

    write_mesh(filename)
    return nhex


# ---------------------------------------------------------------------
# Programme principal
# ---------------------------------------------------------------------

def main():
    init_gmsh()

    for n_target in TARGETS:
        print(f"\n=== Cible : {n_target:,} mailles ===".replace(",", " "))

        print("  Maillage tétraédrique...")
        build_tet_sphere(n_target, f"sphere_tet_{n_target}.msh")

        print("  Maillage hexaédrique...")
        build_hex_sphere(n_target, f"sphere_hex_{n_target}.msh")

    gmsh.finalize()
    print("\nTerminé. Fichiers dans le dossier:", OUTDIR)


if __name__ == "__main__":
    main()
