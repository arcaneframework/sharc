# SolverBench

> 🧠 Documentation enrichie par IA — basée sur l'analyse des scripts Python de ce répertoire

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

`SolverBench` est un outil de génération, exécution et analyse de benchmarks pour les solveurs linéaires utilisés avec Arcane / ArcGeoSim (Alien, PETSc, Hypre, HTSSolver, IFPSolver, MCGSolver, Trilinos, etc.).

Le pipeline complet est :

1. **Génération des cas de tests** à partir d'un jeu de fichiers `.arc` de base et de fichiers de configuration XML.  
   → `generate_tests.py`
2. **Préparation des répertoires d'exécution** et lancement des simulations MPI.  
   → `run_tests.py`
3. **Collecte et post-traitement des résultats solveur** (fichiers XML de log) pour produire des pages HTML synthétiques.  
   → `process_results.py`
4. **Orchestration du pipeline complet** (génération + run + post-traitement).  
   → `generate_run_process.py`

Les cas générés combinent automatiquement différents backends, algorithmes de solveur et préconditionneurs en se basant sur la structure des services Arcane (`*.axl`) et sur un dictionnaire de mapping (`solver_dictionnary.py`).

## Fonctionnalités

- ✅ **Lecture d'une configuration XML unique** (`config_file`) qui décrit :
  - les répertoires d'entrée (`input-directory`),
  - le répertoire de sortie (`output-directory`),
  - l'emplacement d'Alien et des services solveurs (`alien-root`),
  - les packages solveurs à tester (`packages`),
  - les solveurs à modifier dans les cas (`solvers`),
  - les options globales de solveur (`solver-global-options`),
  - l'application (`app-name`) et son binaire (`binary`).

- ✅ **Génération automatique de cas de tests `.arc`** :
  - à partir de cas de base,
  - en injectant des combinaisons (backend, solver, precond),
  - en ajoutant une configuration de logging (`SolverLogger`) pour chaque système linéaire,
  - en créant aussi une version "_original" du cas avec logger mais sans modifications de backend.

- ✅ **Gestion des ressources de test** :
  - copie / symlink des fichiers `.vt2` (maillages),
  - copie / symlink des répertoires `input` / `Input`,
  - recherche de fichiers `app-name.config` dans l'arbre `../src` si absent localement.

- ✅ **Exécution en parallèle des tests** :
  - création de sous-répertoires `np<N>` pour chaque nombre de processus,
  - lancement via `mpirun -np <N> <binary> <case.arc>`,
  - paramétrage de `ARCANE_PARALLEL_SERVICE=Mpi`.

- ✅ **Post-traitement complet des résultats solveur** :
  - parsing des fichiers XML de logs générés par `SolverLogger`,
  - extraction des informations : nombre de proc, package, solver, precond, itérations, succès/erreur, temps d'init, temps de solve, temps total,
  - normalisation des noms de préconditionneurs selon le backend (Petsc, Trilinos, HTS),
  - filtrage des résultats sans itérations (`iterations == '0'`),
  - génération de pages HTML :
    - par cas (`test_cases.html` + pages individuelles),
    - par solveur (`test_solvers.html` + pages individuelles),
  - coloration des lignes suivant le succès/échec (vert/rouge) et cas "original" (gris).

- ✅ **Pages HTML triables** :
  - utilisation de `sorttable.js` pour rendre les colonnes triables,
  - liens depuis une page d'index vers les résultats par cas et par solveur.

- ✅ **Orchestration du pipeline** :
  - script `generate_run_process.py` qui enchaîne :
    - `generate_tests.main()`
    - `run_tests.main()`
    - `process_results.main()`

## Prérequis

- **Python** : 2.x ou 3.x (certains scripts gèrent les deux, ex. `html_utils.py` pour l'écriture HTML).
- **MPI** : exécution via `mpirun` (OpenMPI, MPICH ou équivalent), cohérente avec la configuration Arcane.
- **Arcane / ArcGeoSim / Alien** : installés et accessibles avec les services solveurs correspondants (HypreSolver, PETScSolver, MTLSolver, HTSSolver, IFPSolver, MCGSolver, TrilinosSolver).
- **Arborescence de projet** :
  - cas `.arc` de base dans des sous-répertoires,
  - maillages `.vt2`,
  - configuration `app-name.config` (ou capacité de la retrouver dans `../src`),
  - fichiers de configuration XML (ex. `tests_configuration_arcades.xml`, `tests_configuration_arctem.xml`, `tests_configuration_geoxim.xml`).

## Installation

Les scripts sont déjà dans `common/ArcaneInfra/tools/SolverBench`. Aucune installation Python spécifique n'est codée ici (tout est fichier unique).

Pour les utiliser :

1. Assurer que le **Python** utilisé est celui prévu pour l'environnement Arcane.
2. Vérifier que les chemins dans les fichiers XML (`tests_configuration_*.xml`) pointent vers :
   - `alien-root` correct (répertoire où se trouvent les `*.axl` des solveurs Alien),
   - `base-path` (répertoire des cas `.arc` de base),
   - `input-directory`, `output-directory` cohérents.
3. Lancer les scripts depuis `common/ArcaneInfra/tools/SolverBench` (ils supposent `os.chdir` sur ce répertoire).

Exemple de commande :

```bash
cd common/ArcaneInfra/tools/SolverBench
python generate_run_process.py tests_configuration_arcades.xml
```

(en Python 3, adapter l'appel ou le shebang si nécessaire, le script `generate_run_process.py` est écrit avec `print` Python 2).

## Utilisation

### 1. Fichier de configuration XML

Les fichiers de configuration (ex. `tests_configuration_arcades.xml`) définissent :

- `<app-name>` : nom de l'application (par ex. `ShArc`).
- `<binary>` : chemin du binaire à exécuter.
- `<alien-root>` : racine où chercher les services solveurs (fichiers `.axl`).
- `<base-path>` : répertoire racine des cas `.arc` de base.
- `<input-directory>` : un ou plusieurs répertoires contenant les `.arc` à dupliquer/modifier.
- `<output-directory>` : répertoire de sortie pour les cas générés.
- `<solvers>` : liste de chemins XML décrivant les solveurs à modifier (tag dans les cas).
- `<solver-global-options>` : options à recopier dans les nouveaux solveurs (max-it, tol, etc.).
- `<packages>` : packages solveurs à tester (HypreSolver, PETScSolver, HTSSolver, IFPSolver, MCGSolver, TrilinosSolver).
- `<nb-procs-list>` : liste des nombres de processus MPI pour les exécutions.

### 2. Pipeline en une commande

Script orchestrateur :

```python
# generate_run_process.py
import config
from generate_tests import main as generate_tests
from run_tests import main as run_tests
from process_results import main as process_results

def main():
    print 'Generating test cases'
    generate_tests()
    print 'Running tests'
    run_tests()
    print 'Processing results'
    process_results()

if __name__ == "__main__":
    main()
```

Usage (Python 3 recommandé, conversion `print` à adapter si besoin) :

```bash
python generate_run_process.py tests_configuration_arcades.xml
```

Le module `config.py` parse automatiquement :

```python
parser = argparse.ArgumentParser()
parser.add_argument("config_file", help="File configuration")
parser.add_argument("verbose", nargs='?', type=int, default=0, help="verbose mode")
args = parser.parse_args()

verbose = args.verbose
config_file = args.config_file
```

### 3. Génération des cas : `generate_tests.py`

- Recherche les cas `.arc` dans les `<input-directory>` via `get_files_by_extension`.
- Crée, pour chaque cas, une version `_original` avec un logger attaché (`add_logger_to_original_arcfile`).
- Construit un dictionnaire de mapping de paramètres par backend (`build_solver_dictionnary`).
- Pour chaque package solveur :
  - détecte les solveurs et préconditionneurs disponibles en parcourant `alien-root` et en lisant les `.axl` (`get_avail_solvers_preconds`),
  - pour chaque cas `.arc` :
    - modifie les sous-blocs solveurs ciblés,
    - ajoute les balises `<solver>`, `<preconditioner>` ou options spécifiques,
    - ajoute un logger avec nom de fichier unique,
    - écrit les nouveaux `.arc` dans `output-directory`, avec un suffixe combinant `package`, `solver`, `precond`.

### 4. Exécution des tests : `run_tests.py`

- Relit `config_file`, récupère :
  - `output-directory`,
  - `app-name`,
  - `nb-procs-list`,
  - `binary`.
- Pour chaque `.arc` dans `output-directory` :
  - crée un sous-répertoire `np<nb_procs>` (ex. `np4`),
  - symlink les ressources (`input`, `Input`, `.vt2`, `<app-name>.config`),
  - lance `mpirun -np <nb_procs> <binary> <case.arc>`, redirige la sortie vers `output_<casename>`,
  - nettoie les symlinks et répertoire `output/` après chaque run.

### 5. Post-traitement : `process_results.py`

- Recherche les fichiers `.xml` de sortie sur `output-directory`.
- Construit une liste `TestCase` avec :
  - `case`, `nb_proc`, `solver_name`, `package`,
  - `max_it`, `tol`, `solver`, `precond`,
  - `init_time`, `calls`, `success`, `error`, `iterations`, `solve_time`, `total_time`.
- Normalise les noms de préconditionneur (`actual_precond_name`) pour certains backends :
  - PETSc (`hypre-amg` → `hypre`),
  - Trilinos (`riluk` → `iluk`, `fast_ilu` → `filu`),
  - HTSSolver (`chebyshev` → `poly` si nom absent dans le filename).
- Génère :
  - `html/test_cases.html` et une page HTML par cas,
  - `html/test_solvers.html` et une page HTML par solveur,
  - `html/index.html` formant une page d'entrée avec liens vers les deux vues.
- Copie `sorttable.js` dans `html/sorttable.js` pour rendre les tableaux triables.

## API / Interface

Résumé des principales fonctions/classes :

| Symbole | Type | Description |
|---------|------|-------------|
| `config.config_file` | variable | Chemin du fichier de configuration XML passé en argument sur la ligne de commande. |
| `config.verbose` | variable | Niveau de verbosité (0 silencieux, >0 affiche les détails des packages/solvers). |
| `generate_tests.main()` | fonction | Orchestrateur de la génération des cas `.arc` modifiés + cas `_original`. |
| `run_tests.main()` | fonction | Prépare les répertoires `np<N>` et lance les simulations MPI pour chaque cas généré. |
| `process_results.main()` | fonction | Parse les logs solveurs, normalise les noms, filtre les résultats invalides et génère les pages HTML. |
| `io_utils.indent(elem, level, more_sibs)` | fonction | Formate proprement l'XML (indentation) avant écriture. |
| `io_utils.get_files_by_extension(input_directories, extension)` | fonction | Retourne la liste des fichiers ayant une extension donnée dans tous les `<input-directory>`. |
| `io_utils.create_tests_ressources(app_name, base_path, output_directory, filelist)` | fonction | Copie/symlink les ressources nécessaires pour chaque cas (maillages, config) et crée les cas `_original`. |
| `html_utils.create_main_page()` | fonction | Crée la page d'index HTML avec liens vers résultats par cas et par solveur. |
| `html_utils.create_result_page_header()` | fonction | Crée le header HTML pour une page de résultats (incl. script `sorttable.js`). |
| `html_utils.create_a_table_header(h2, header)` | fonction | Crée un tableau HTML triable avec en-tête défini. |
| `solver_dictionnary.build_solver_dictionnary(solver_dict)` | fonction | Remplit un dictionnaire qui mappe les noms de tags de configuration génériques vers ceux spécifiques à chaque backend (Hypre, PETSc, MTL, HTS, IFPSolver, MCGSolver, Trilinos). |

## Architecture

```text
┌───────────────────────────────────────────────────────────────┐
│                        SolverBench                            │
├───────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌────────────────────┐   ┌───────────────────────┐           │
│  │ config.py          │   │ tests_configuration*.xml │        │
│  │ (CLI, config_file) │   │ (paramétrage global)   │          │
│  └────────────────────┘   └───────────────────────┘           │
│           │                          │                        │
│           ▼                          ▼                        │
│  ┌──────────────────────────────┐    ┌────────────────────┐    │
│  │ generate_tests.py           │    │ solver_dictionnary.py │  │
│  │ (cas .arc générés + _original) │ │ (mapping backend/tags) │ │
│  └──────────────────────────────┘    └────────────────────┘    │
│           │                          │                        │
│           ▼                          ▼                        │
│  ┌──────────────────────────────┐                               │
│  │ run_tests.py                 │                               │
│  │ (np<N> dirs, mpirun)         │                               │
│  └──────────────────────────────┘                               │
│           │                                                   │
│           ▼                                                   │
│  ┌──────────────────────────────┐   ┌────────────────────┐    │
│  │ process_results.py           │   │ html_utils.py       │    │
│  │ (parse XML logs, pages HTML) │   │ (construction HTML) │    │
│  └──────────────────────────────┘   └────────────────────┘    │
│           │                                                   │
│           ▼                                                   │
│     html/ (index, test_cases, test_solvers)                   │
│                                                               │
└───────────────────────────────────────────────────────────────┘
         │
         ▼
   Exécutions ShArc / autres applis
   (binaire MPI, cas .arc générés)
```

## Dépendances

| Dépendance | Usage |
|------------|-------|
| `xml.etree.ElementTree` | Parsing et génération XML (configuration, logs solveurs, pages HTML). |
| `os`, `glob`, `shutil` | Gestion des fichiers et répertoires, symlinks, copies de ressources. |
| `argparse` | Parsing des arguments CLI (`config_file`, `verbose`). |
| `re` | Substitutions insensibles à la casse (`ireplace`). |
| `mpirun` (MPI) | Lancement des tests parallèles. |
| Backends solveurs Arcane/Alien | Hypre, PETSc, MTL, HTS, IFPSolver, MCGSolver, Trilinos (via fichiers `.axl` et services). |

## Configuration

Exemple de paramètres (conceptuels) dans un fichier `tests_configuration_*.xml` :

| Tag | Description |
|-----|-------------|
| `<app-name>` | Nom de l'application (par ex. `ShArc`). |
| `<binary>` | Chemin du binaire (`ShArc.exe`, etc.). |
| `<alien-root>` | Racine où se trouvent les `.axl` des solveurs. |
| `<base-path>` | Répertoire des cas `.arc` de base. |
| `<input-directory>` | Un ou plusieurs répertoires contenant les cas `.arc` à décliner. |
| `<output-directory>` | Répertoire de sortie des cas générés et des logs XML. |
| `<packages>` | Liste de `<package name="HypreSolver">...</package>` etc. |
| `<solvers>` | Liste de tags décrivant où se trouvent les blocs solveur dans le XML des cas. |
| `<solver-global-options>` | Options à recopier dans chaque nouveau bloc `solver`/`precond`. |
| `<nb-procs-list>` | Liste de `<nb-procs-list><nb-proc>4</nb-proc>...</nb-proc-list>` pour les runs MPI. |

## Ressources

- Fichiers de configuration fournis :  
  - `tests_configuration_arcades.xml`  
  - `tests_configuration_arctem.xml`  
  - `tests_configuration_geoxim.xml`
- Script d'exemple `submit_cluster_exemple.sh` — illustration d'usage sur un cluster HPC.
- Fichier `sorttable.js` — script JS pour rendre les tableaux HTML triables.
