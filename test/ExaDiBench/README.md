# EXADI BENCHMARCK

## Objectives

This directory is aimed at benchmarcking efficient Arcane Mesh reader for exascale meshes, in particular

the msh and meshb reader are in particular

## Tools to generate unit cube and sphere meshes

### Generate Unit cube meshes
```bash
uv init cube-meshing
cd cube-meshing
uv add gmsh meshio
uv run python ../cube_meshes.py
```

### generate Unit sphere meshes
```bash
uv init sphere-meshing
cd sphere-meshing
uv add gmsh meshio
uv run python ../sphere_meshes.py
```

## Get meshes from ZENODO with git-annex


### Install GIT-ANNEX
Some commands to install git-annex with guix, conda or uv

```bash
# GUIX
guix install git-annex

#CONDA
conda install -c conda-forge git-annex

#UV
uv tool install git-annex
```

For more details please refer to the Git [Annex installation guide](https://git-annex.branchable.com/install/).


### Retreving meshes files

```bash
cd /path_to/sharc
git annex init 'SHARC on My local computer'
cd ./test/ExaDIxaDiBench/meshes
git annex get .
```

### Check mesh file list

```bash
git annex list

here
|origin
||web
|||bittorrent
||||
X_X_ unit_cube_hex_1m.meshb
X_X_ unit_cube_hex_1m.msh
X_X_ unit_cube_hex_212k.meshb
X_X_ unit_cube_hex_212k.msh
X_X_ unit_cube_hex_24m.msh
X_X_ unit_cube_hex_28k.meshb
X_X_ unit_cube_hex_28k.msh
X_X_ unit_cube_tet_1m.meshb
X_X_ unit_cube_tet_1m.msh
X_X_ unit_cube_tet_212k.meshb
X_X_ unit_cube_tet_212k.msh
X_X_ unit_cube_tet_24m.meshb
X_X_ unit_cube_tet_24m.msh
X_X_ unit_cube_tet_28k.meshb
X_X_ unit_cube_tet_28k.msh

```

```bash
git annex whereis

whereis unit_cube_hex_1m.meshb (2 copies) 
    00000000-0000-0000-0000-000000000001 -- web
    bcc01c6b-a038-4932-abef-ee0f90321e8e -- LocalLaptop [here]

  web: https://zenodo.org/records/23000296/files/unit_cube_hex_1m.meshb?download=1
ok
whereis unit_cube_hex_1m.msh (2 copies) 
    00000000-0000-0000-0000-000000000001 -- web
    bcc01c6b-a038-4932-abef-ee0f90321e8e -- LocalLaptop [here]

  web: https://zenodo.org/records/23000296/files/unit_cube_hex_1m.msh?download=1
ok
whereis unit_cube_hex_212k.meshb (2 copies) 
    00000000-0000-0000-0000-000000000001 -- web
    bcc01c6b-a038-4932-abef-ee0f90321e8e -- LocalLaptop [here]

  web: https://zenodo.org/records/23000296/files/unit_cube_hex_212k.meshb?download=1
ok

```

### Drop mesh files

```bash
cd /path_to/sharc/test/ExaDIBench/meshes
git annex drop *.msh
git annex drop *.meshb
```
