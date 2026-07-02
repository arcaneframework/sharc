# ShArc

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

Le module `ShArc` est l'orchestrateur principal de l'application ShArc. Il ne porte pas directement la physique (modèles de flux diphasique, FEM, etc.), mais coordonne :

- les services ArcGeoSim/Arcane de temps (`ITimeLine`, `ITimeStepComputer`),
- la gestion de la géométrie du maillage (`IGeometryMng`),
- le système d'unités (`IUnitsSystem`),
- le gestionnaire d'expressions (`IExpressionMng`),
- le post-traitement (sorties périodiques, fichiers de dépouillement),
- la validation de la simulation via des services `IServiceValidator`.

`ShArcModule` dérive de `ArcaneShArcObject`, implémente `IAppServiceMng` (pour enregistrer/trouver les services applicatifs) et délègue des informations d'itération via `ArcGeoSim::Delegate<ArcGeoSim::IIterationInformation>`. Il agit comme "time loop orchestrator" : initialisation, déroulement des pas de temps, déclenchement du post-processing, arrêt contrôlé de la boucle de calcul (succès/échec).

## Fonctionnalités

- ✅ Gestion centralisée des services applicatifs :
  - time manager (`options()->timeManager`),
  - politique de pas de temps (`options()->timeStepPolicy`),
  - géométrie (`options()->geometry`),
  - système d'unités (`options()->unitsSystem`),
  - expression manager (`options()->expressionMng`).

- ✅ Intégration avec la boucle en temps Arcane :
  - détection du mode spécial `ForecastCompute`,
  - contrôle du `timeLoopMng()` pour arrêter proprement la simulation.

- ✅ Support du maillage évolutif / connectivités :
  - préparation de la connectivité "edge" si nécessaire (`options()->hasEdges()`),
  - mise en place d'une politique de géométrie (`NoOptimizationGeometryPolicy`) et mise à jour de la géométrie.

- ✅ Orchestration du post-traitement :
  - initialisation du post-processing (chemins de sortie, formats, variables),
  - sorties périodiques en fonction d'un `outputPeriod`,
  - sauvegarde de l'état à l'initialisation si demandé (`saveInit()`),
  - génération des fichiers de dépouillement et statistiques en fin de simulation.

- ✅ Validation globale de la simulation :
  - gestion d'une liste de `IServiceValidator`,
  - exécution de `validate()` sur chaque validateur,
  - arrêt de la simulation en cas de validation échouée, avec message explicite.

- ✅ Arrêt contrôlé de la boucle de calcul :
  - arrêt sur nombre maximum d'itérations (`maxIteration`),
  - arrêt lorsque la timeline n'est plus disponible,
  - distinction entre "STOP PERFECT" (succès) et "STOP FAILED" (échec).

- ✅ Journalisation détaillée du temps :
  - fonction `timeStepInformation()` qui logge `T(n)`, `T(n+1)` et `Δt` avec une précision configurable.

## Prérequis

- Langage : C++ (mêmes contraintes que le reste du projet ShArc).
- Frameworks :
  - Arcane (modules, time loop, maillage, post-processing).
  - ArcGeoSim (services d'units, temps, géométrie, expressions, architecture "Delegate").
- Types et services utilisés :
  - `ArcGeoSim/Appli/AppService.h` pour la gestion des services.
  - `ArcGeoSim/Mesh/Geometry/IGeometryMng.h` et `IGeometryPolicy`.
  - `ArcGeoSim/Time/ITimeLine.h`, `TimeEvolution`, `ITimeStepComputer`.
  - `ArcGeoSim/Physics/Units/IUnitsSystem.h`.
  - `ArcGeoSim/Numerics/Expressions/IExpressionMng.h`.
  - `ArcGeoSim/Tests/IServiceValidator.h`.

Le module est déjà intégré dans la librairie `ShArc` via `src/CMakeLists.txt`. Aucun CMakeLists spécifique n'est à créer ici.

## Installation

L'installation suit celle du projet ShArc. Le module `ShArc` est compilé avec le reste de la librairie `ShArc` :

```bash
# Depuis la racine du projet sharc
cmake -S . -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR} \
  -DCMAKE_C_COMPILER=mpicc \
  -DCMAKE_CXX_COMPILER=mpic++

cmake --build build-sharc
```

L'exécutable principal (`ShArc.exe`) est défini dans `src/Main/` et utilise ce module via ses fichiers `.axl` (`ShArc.axl`) et les cas `.arc`.

## Utilisation

### Cycle de vie du module

La classe `ShArcModule` déclare les principales méthodes de cycle de vie :

```cpp
class ShArcModule
  : public ArcaneShArcObject
  , public IAppServiceMng
  , public ArcGeoSim::Delegate<ArcGeoSim::IIterationInformation>
{
public:
  ShArcModule(const Arcane::ModuleBuildInfo& mbi);
  virtual ~ShArcModule();

  void build();
  void prepareInit();
  void init();
  void continueInit();
  void endInit();
  void beginTimeStep();
  void endTimeStep();
  void endSimulation();

  Arcane::VersionInfo versionInfo() const { return Arcane::VersionInfo(1,0,0); }
};
```

Séquence typique :

1. `build()`  
   - Détecte si la simulation est en mode `ForecastCompute` (time loop nommé "ForecastCompute").  
   - Prépare la connectivité du maillage pour le "worst case" (inclusion des arêtes) si nécessaire.

2. `prepareInit()`  
   - Initialise le gestionnaire de services applicatifs (`initializeAppServiceMng()`),  
   - récupère `IGeometryMng` et met en place une `NoOptimizationGeometryPolicy`.

3. `init()`  
   - Initialise la politique de pas de temps (`ITimeStepComputer`) si configurée,  
   - enregistre le compute de pas de temps auprès de la timeline (`ITimeLine::evolution()`),  
   - effectue une mise à jour initiale de la géométrie (`m_geometry_mng->update(...)`),  
   - initialise le post-processing (`postProcessingInit()`),  
   - appelle `init()` sur tous les validateurs (`options()->validator`).

4. `endInit()`  
   - Effectue une sauvegarde initiale (`saveInit()`) si demandé dans les options de post-processing.

5. `beginTimeStep()`  
   - Vérifie que le module est correctement initialisé (`ARCANE_ASSERT(m_initialized)`).

6. `endTimeStep()`  
   - Gère le post-processing périodique en fonction de `outputPeriod`,  
   - teste les critères d'arrêt (maxIteration, timeline) et arrête la boucle de calcul via `timeLoopMng()->stopComputeLoop(...)`.

7. `endSimulation()`  
   - Appelle `postProcessingExit()` (sortie finale, statistiques, fermeture des writers),  
   - lance la séquence de validation `_validate()` qui arrête la simulation en "success" ou "failure".

### Post-traitement

Le post-processing repose sur `ArcGeoSimPostProcessingBase` (`m_post_processing`) :

- `postProcessingInit()` :
  - détermine le chemin de sortie (`outputPath` ou `caseName()`),
  - enregistre les post-processeurs (formats, variables) via `addPostProcessing(...)`,
  - initialise `m_next_iteration` pour déterminer la prochaine sortie.

- `postProcessingExit()` :
  - sauvegarde les données au temps courant,
  - affiche les statistiques d'exécution,
  - ferme les writers (`IPostProcessorWriter::close()`).

### Validation

La méthode `_validate()` :

```cpp
void ShArcModule::_validate()
{
  bool hasError = false;

  const Integer count = options()->validator.size();
  for(Integer i=0;i<count;++i) {
    if (options()->validator[i]->validate() != 0) {
      hasError = true;
      perror() << "ShArc validator #" << i << " failed";
    }
  }

  if (hasError) {
    fatal() << "ShArc validation failure : read details above";
    info() << "STOP FAILED";
    subDomain()->timeLoopMng()->stopComputeLoop(false,true);
  }
  else {
    info() << "STOP PERFECT";
    subDomain()->timeLoopMng()->stopComputeLoop(true);
  }
}
```

Ainsi, le module centralise les validations de fin de simulation et contrôle l'arrêt du calcul.

## API / Interface publique

| Symbole | Type | Description |
|---------|------|-------------|
| `ShArcModule::build()` | méthode de module | Prépare le mode `ForecastCompute` et les connectivités du maillage (edges). |
| `ShArcModule::prepareInit()` | méthode de module | Initialise les services applicatifs et la politique de géométrie (`IGeometryMng`, `IGeometryPolicy`). |
| `ShArcModule::init()` | méthode de module | Initialise la politique de pas de temps, la mise à jour géométrique, le post-processing et les validateurs. |
| `ShArcModule::continueInit()` | méthode de module | Assure que la gestion des services est initialisée (pour des scénarios de ré-init). |
| `ShArcModule::endInit()` | méthode de module | Sauvegarde d'init (`saveInit`) si configurée, via le post-processing. |
| `ShArcModule::beginTimeStep()` | méthode de module | Vérifie que `ShArc` est initialisé avant d'entrer dans un pas de temps. |
| `ShArcModule::endTimeStep()` | méthode de module | Gère les sorties périodiques, l'arrêt sur itération max ou sur timeline, et journalise "STOP PERFECT". |
| `ShArcModule::endSimulation()` | méthode de module | Déclenche le post-processing final et appelle `_validate()` pour décider du statut final. |
| `ShArcModule::timeStepInformation(nb_loop)` | méthode | Affiche les informations de temps (T(n), T(n+1), Δt) avec un formatage précis. |
| `ShArcModule::updateGeometry()` | méthode | Met à jour la géométrie via `IGeometryMng` et la politique associée. |
| `ShArcModule::versionInfo()` | méthode | Retourne la version (1.0.0) du module ShArc. |

## Architecture

```text
┌───────────────────────────────────────────────────────────────┐
│                         ShArcModule                          │
├───────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌────────────────────┐    ┌─────────────────────────┐        │
│  │ Time Manager       │    │ Time Step Policy        │        │
│  │ (ITimeLine)        │    │ (ITimeStepComputer)     │        │
│  └────────────────────┘    └─────────────────────────┘        │
│          │                          │                         │
│          ▼                          ▼                         │
│  ┌────────────────────┐    ┌─────────────────────────┐        │
│  │ GeometryMng        │    │ ExpressionMng           │        │
│  │ (IGeometryMng)     │    │ (IExpressionMng)        │        │
│  └────────────────────┘    └─────────────────────────┘        │
│          │                          │                         │
│          ▼                          ▼                         │
│  ┌────────────────────┐    ┌─────────────────────────┐        │
│  │ Units System       │    │ Validators              │        │
│  │ (IUnitsSystem)     │    │ (IServiceValidator)     │        │
│  └────────────────────┘    └─────────────────────────┘        │
│                 │                     │                       │
│                 ▼                     ▼                       │
│          ┌───────────────────────────────────┐                │
│          │ ArcGeoSimPostProcessingBase       │                │
│          │ (post-processing, output, stats)  │                │
│          └───────────────────────────────────┘                │
│                                                               │
└───────────────────────────────────────────────────────────────┘
         │                        │
         ▼                        ▼
   Arcane TimeLoopMng       Services ArcGeoSim/AppService
 (contrôle stop/continue)   (enregistrement & lookup)
```

## Dépendances

| Dépendance | Usage |
|------------|-------|
| Arcane | Time loop (`ITimeLoopMng`, `ITimeLoop`), maillage (`IMesh`), partitionnement, post-processing (`IPostProcessorWriter`), logging. |
| ArcGeoSim | Gestion des services (`AppService`), unités (`IUnitsSystem`), temps (`ITimeLine`, `TimeEvolution`, `ITimeStepComputer`), géométrie (`IGeometryMng`, `IGeometryPolicy`), expressions (`IExpressionMng`), architecture (`Delegate`, `IIterationInformation`), tests (`IServiceValidator`). |

## Configuration

Les options du module sont définies dans `ShArc.axl` et utilisées via `options()` :

Exemples conceptuels d'options :

| Option | Description |
|--------|-------------|
| `timeManager` | Service de gestion du temps (`ITimeLine`), utilisé pour enregistrer le compute de pas de temps. |
| `timeStepPolicy` | Politique de pas de temps (`IComputer`), appelée pour initialiser le pas de temps et enregistrée dans `TimeEvolution`. |
| `geometry` | Service de gestion de la géométrie (`IGeometryMng`), utilisé pour charger/mettre à jour les propriétés géométriques. |
| `unitsSystem` | Système d'unités pour la simulation (SI, mixte…), utilisé par les modules physiques. |
| `expressionMng` | Gestionnaire d'expressions (parser, évaluations) pour IC/BC/Wells et autres lois. |
| `postProcessing` | Bloc de configuration du post-traitement (période de sortie, chemin, formats, variables). |
| `validator` | Liste de services de validation, chacun exposant une méthode `validate()` retournant un code d'erreur. |
| `maxIteration` | Nombre maximal d'itérations globales ; au-delà, la simulation est arrêtée. |
| `hasEdges` | Indique si la simulation requiert la connectivité "edges" pour le maillage (maillage évolutif ou discrétisations spécifiques). |

Les valeurs exactes et la syntaxe des options sont à lire dans `ShArc.axl` et les cas `.arc` du projet.

## Ressources

- [ShArc (GitHub)](https://github.com/arcaneframework/sharc) — dépôt principal, README d'installation détaillé.
- Documentation Arcane/ArcGeoSim dans `common/` et sur les pages projet (services, time loop, géométrie).
- Cas de test du projet (dans `test/`) montrant l'utilisation du module ShArc pour orchestrer différentes applications physiques (TwoPhaseFlow, CAWF, ExaDiBench).
