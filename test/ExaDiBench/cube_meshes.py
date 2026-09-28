#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Génération de maillages du cube unité [0,1]^3 avec Gmsh (API Python),
en tétraèdres et en hexaèdres, pour plusieurs tailles cibles,
export en .msh binaire ET en .meshb binaire (format INRIA/Medit).

Cibles : 28 000 / 212 000 / 1 000 000 / 24 000 000 mailles

Principe :
----------
- TÉTRAÈDRES : cube plein (OCC) + champ de taille uniforme "h", ajusté par
  quelques itérations (h ~ (6*sqrt(2)*V/N)^(1/3)) pour converger vers le
  nombre de mailles cible.

- HEXAÈDRES : le cube est un cas trivial pour un maillage 100% structuré :
  transfinite sur les 12 arêtes / 6 faces / le volume, avec recombine.
  n = round(N^(1/3)) subdivisions par arête -> n^3 hexaèdres exactement.

Note sur le format .meshb :
----------------------------
Gmsh (build officiel PyPI) ne sait écrire le format Medit qu'en ASCII
(.mesh), pas en binaire (.meshb) -> "Unknown output file format" si on
essaie gmsh.write("*.meshb"). On passe donc par la librairie `meshio`
pour produire le binaire .meshb à partir du maillage généré par Gmsh
(on ne garde que les points et les éléments volumiques, sans les
métadonnées Gmsh qui font échouer l'écriture binaire Medit).

Installation requise :
    pip install gmsh meshio

Utilisation :
    python3 cube_meshes.py
Les fichiers sont écrits dans ./meshes/
"""

import gmsh
import meshio
import math
import os

# ---------------------------------------------------------------------
# Paramètres généraux
# ---------------------------------------------------------------------

TARGETS = [28_000, 212_000, 1_000_000, 24_000_000]
SIDE = 1.0                 # côté du cube
OUTDIR = "meshes"

MSH_VERSION = 4.1          # 2.2 = compatible avec la quasi-totalité des solveurs


# ---------------------------------------------------------------------
# Utilitaires communs
# ---------------------------------------------------------------------

def init_gmsh():
    gmsh.initialize()
    gmsh.option.setNumber("General.Terminal", 1)
    gmsh.option.setNumber("General.Verbosity", 2)
    gmsh.option.setNumber("Mesh.Binary", 1)          # écriture .msh binaire
    gmsh.option.setNumber("Mesh.MshFileVersion", MSH_VERSION)


def count_elements(dim):
    etypes, etags, _ = gmsh.model.mesh.getElements(dim)
    return sum(len(t) for t in etags)


def write_msh_binary(basename):
    os.makedirs(OUTDIR, exist_ok=True)
    path = os.path.join(OUTDIR, basename + ".msh")
    gmsh.write(path)
    print(f"    -> écrit : {path}")
    return path


def write_meshb_binary(msh_path, cell_type, basename):
    """Relit le .msh via meshio, ne garde que les éléments volumiques
    (tetra ou hexahedron) + les points, et écrit un .meshb binaire propre."""
    os.makedirs(OUTDIR, exist_ok=True)
    m = meshio.read(msh_path)

    cells = None
    for cb in m.cells:
        if cb.type == cell_type:
            cells = cb.data
            break
    if cells is None:
        raise RuntimeError(f"Aucune cellule de type '{cell_type}' trouvée dans {msh_path}")

    clean = meshio.Mesh(points=m.points, cells=[(cell_type, cells)])
    path = os.path.join(OUTDIR, basename + ".meshb")
    meshio.write(path, clean)  # l'extension .meshb suffit à sélectionner l'écriture binaire Medit
    print(f"    -> écrit : {path}")
    return path


# ---------------------------------------------------------------------
# 1) Maillage tétraédrique du cube plein
# ---------------------------------------------------------------------

def build_tet_cube(n_target, basename, max_iter=6, tol=0.02):
    gmsh.model.add(f"cube_tet_{n_target}")
    gmsh.model.occ.addBox(0, 0, 0, SIDE, SIDE, SIDE)
    gmsh.model.occ.synchronize()

    gmsh.option.setNumber("Mesh.MeshSizeFromCurvature", 0)
    gmsh.option.setNumber("Mesh.Algorithm3D", 10)   # HXT : rapide, adapté aux gros volumes

    V = SIDE ** 3
    # volume moyen d'un tétraèdre "régulier" d'arête h : h^3/(6*sqrt(2))
    h = (V * 6.0 * math.sqrt(2.0) / n_target) ** (1.0 / 3.0)

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

    msh_path = write_msh_binary(basename)
    #write_meshb_binary(msh_path, "tetra", basename)
    return ntet


# ---------------------------------------------------------------------
# 2) Maillage hexaédrique du cube plein (structuré, exact)
# ---------------------------------------------------------------------

def build_hex_cube(n_target, basename):
    n = max(1, round(n_target ** (1.0 / 3.0)))
    print(f"    [hex] n={n} subdivisions/arête -> {n**3} hexas (cible {n_target})")

    gmsh.model.add(f"cube_hex_{n_target}")
    gmsh.model.occ.addBox(0, 0, 0, SIDE, SIDE, SIDE)
    gmsh.model.occ.synchronize()

    for (dim, tag) in gmsh.model.getEntities(1):
        gmsh.model.mesh.setTransfiniteCurve(tag, n + 1)
    for (dim, tag) in gmsh.model.getEntities(2):
        gmsh.model.mesh.setTransfiniteSurface(tag)
        gmsh.model.mesh.setRecombine(2, tag)
    for (dim, tag) in gmsh.model.getEntities(3):
        gmsh.model.mesh.setTransfiniteVolume(tag)

    gmsh.model.mesh.generate(3)
    nhex = count_elements(3)
    print(f"    [hex] maillage généré : {nhex} hexaèdres "
          f"(écart {(nhex/n_target-1)*100:+.1f}%)")

    msh_path = write_msh_binary(basename)
    #write_meshb_binary(msh_path, "hexahedron", basename)
    return nhex


# ---------------------------------------------------------------------
# Programme principal
# ---------------------------------------------------------------------

def main():
    init_gmsh()

    for n_target in TARGETS:
        print(f"\n=== Cible : {n_target:,} mailles ===".replace(",", " "))

        print("  Maillage tétraédrique...")
        build_tet_cube(n_target, f"cube_tet_{n_target}")

        print("  Maillage hexaédrique...")
        build_hex_cube(n_target, f"cube_hex_{n_target}")

    gmsh.finalize()
    print("\nTerminé. Fichiers dans le dossier:", OUTDIR)


if __name__ == "__main__":
    main()
