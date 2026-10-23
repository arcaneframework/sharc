# Bench des lecteurs de maillage : meshb (Ouranos) vs msh (Arcane)

Compare, sur les mêmes maillages, le lecteur meshb de ShArc (lecture et partition par
Ouranos) et le lecteur msh parallèle d'Arcane (lecture puis partition par Arcane).

## Prérequis

- Arcane sur la branche `dev/ExaDiBench` (contient les timers du lecteur msh).
- ShArc sur la branche `dev/ExaDiBench`, compilé en Release.
- Les maillages du bench, récupérés avec git-annex : voir [../README.md](../README.md),
  section *Get meshes from ZENODO with git-annex*.

## Chemins

Par défaut, `bench.sh` utilise `build/bin/ShArc.exe` à la racine du dépôt.
Pour un autre build, changer `SHARC_EXE` en haut de `bench.sh` :

```bash
SHARC_EXE=/chemin/vers/build/bin/ShArc.exe
```

## Utilisation

```bash
cd test/ExaDiBench/bench
./bench.sh
python3 plot.py
```

Les résultats sont écrits dans `results/` : `results.csv` et `steps.png`.

Réglages en haut de `bench.sh` : `MESH_FILES` (maillages, chaque meshb suivi de son msh),
`NPROCS` (nombres de rangs MPI) et `REPEATS` (répétitions).

**mpirun / srun :** les runs sont lancés avec `mpirun --bind-to core` (un rang fixé par
cœur). Sur un cluster avec Slurm, remplacer cette ligne de `bench.sh` par l'équivalent
`srun`.
