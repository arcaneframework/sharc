# ShArc (module d'orchestration)

> 🧠 Documentation enrichie par IA — basée sur l'analyse du code source et des descripteurs Arcane

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

Le module `ShArc` est le **module d’orchestration global** de l’application ShArc. Il ne résout pas lui‑même d’équations physiques : il pilote le **cycle de vie de la simulation** Arcane/ArcGeoSim et centralise :

- l’initialisation des services d’application (gestion du temps, politique de pas de temps, géométrie, unités, expressions),
- la préparation du maillage (connectivité, edges),
- le **post‑traitement** (sorties périodiques, formats de sortie, statistiques),
- la **validation** de la simulation en fin de calcul via des services de type `IServiceValidator`,
- les conditions d’arrêt (nombre maximum d’itérations, fin de timeline).

`ShArcModule` est enregistré comme module Arcane via `ARCANE_REGISTER_MODULE_SHARC(ShArcModule)` et ses points d’entrée sont décrits dans `ShArc.axl` (build/init/compute‑loop/exit).

## Fonctionnalités

- ✅ Gestion des services d’application ArcGeoSim :
  - `IUnitsSystem` (système d’unités),
  - `ArcGeoSim::ITimeLine` (gestionnaire de temps),
  - `ArcGeoSim::TimeStep::IComputer` (politique de pas de temps),
  - `IGeometryMng` (gestionnaire de géométrie),
  - `IExpressionMng` (gestionnaire d’expressions).

- ✅ Initialisation de la simulation :
  - Détection du mode `ForecastCompute` (prévision) via le `TimeLoop`.
  - Préparation de la connectivité du maillage (activation des edges si nécessaire).
  - Initialisation de la politique de pas de temps et enregistrement auprès du `TimeLine`.
  - Mise à jour initiale de la géométrie.

- ✅ Pilotage du cycle temporel :
  - `beginTimeStep()` / `endTimeStep()` exécutés à chaque itération.
  - Traces de **time report** détaillées (T(n), T(n+1), Δt) via `timeStepInformation()`.

- ✅ Post‑traitement :
  - Configuration d’un ou plusieurs post‑processeurs (`IPostProcessorWriter`) via `postProcessing`.
  - Gestion de la fréquence de sortie (`outputPeriod`) et du chemin de sortie (`outputPath`).
  - Sauvegarde des champs à des instants donnés et génération de statistiques en fin de simulation.
  - Fermeture propre des writers de post‑traitement.

- ✅ Validation en fin de simulation :
  - Appel des services `IServiceValidator` configurés.
  - Arrêt de la boucle de calcul avec succès ou échec explicite :
    - message `STOP PERFECT` en cas de succès,
    - `STOP FAILED` et `fatal()` en cas d’échec de validation.

- ✅ Conditions d’arrêt :
  - Arrêt après `max-iteration` itérations si la timeline n’est pas disponible.
  - Arrêt basé sur la timeline (`ITimeLine`) si disponible et `max-iteration <= 0`.

## Prérequis

- Langage / frameworks :
  - C++ (Arcane, ArcGeoSim),
  - Module Arcane décrit via `ShArc.axl`.

- Services ArcGeoSim utilisés :
  - `IUnitsSystem`,
  - `ArcGeoSim::ITimeLine`,
  - `ArcGeoSim::TimeStep::IComputer`,
  - `IGeometryMng`,
  - `IExpressionMng`,
  - `IServiceValidator`,
  - `ArcGeoSimPostProcessingBase`.

- Build :
  - Le module est déclaré dans `config.xml` :
    ```xml
    <files language="module">
      <file>ShArc</file>
    </files>
    ```
  - Il est intégré à la bibliothèque principale ShArc via les CMakeLists de `src/`.

## Installation

Aucune étape spécifique pour ce module : il est compilé automatiquement lorsque la bibliothèque ShArc est construite, via la configuration CMake globale (voir README racine).

```bash
cmake -S sharc -B build-sharc \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DARCANEFRAMEWORK_ROOT=${INSTALL_DIR} \
  -DUSE_ARCANE_V3=ON
cmake --build build-sharc
```

Le module est disponible dans `ShArc.exe` et auto‑chargé selon les points d’entrée définis dans `ShArc.axl`.

## Utilisation

`ShArc` est généralement utilisé comme **module d’infrastructure** : il est activé dans les fichiers `.arc` pour fournir les services communs de temps, géométrie, post‑traitement et validation, tandis que les modules physiques (par exemple `TwoPhaseFlowSimulation`, `ThermoChemicalConvection`) gèrent la physique.

### Cycle de vie

Les points d’entrée sont définis dans `ShArc.axl` :

```xml
<entry-points>
  <entry-point method-name="build"         name="Build"         where="build"         property="auto-load-begin" />
  <entry-point method-name="prepareInit"   name="PrepareInit"   where="init"          property="auto-load-begin" />
  <entry-point method-name="init"          name="Init"          where="init"          property="none" />
  <entry-point method-name="continueInit"  name="ContinueInit"  where="continue-init" property="none" />
  <entry-point method-name="endInit"       name="EndInit"       where="init"          property="auto-load-end" />
  <entry-point method-name="beginTimeStep" name="BeginTimeStep" where="compute-loop"  property="auto-load-begin" />
  <entry-point method-name="endTimeStep"   name="EndTimeStep"   where="compute-loop"  property="auto-load-end" />
  <entry-point method-name="endSimulation" name="EndSimulation" where="exit"          property="auto-load-begin"/>
</entry-points>
```

En pratique :

- `build()` prépare le maillage (connectivité, edges) et détecte le mode Forecast.
- `prepareInit()` et `init()` initialisent services et géométrie, ainsi que la politique de pas de temps.
- `endInit()` peut déclencher un post‑traitement initial si `save-init` est `true`.
- `beginTimeStep()` / `endTimeStep()` entourent chaque itération de la boucle de calcul.
- `endSimulation()` lance le post‑traitement final et la validation.

## API / Interface

### Classe principale

| Symbole | Type | Description |
|---------|------|-------------|
| `ShArcModule` | classe | Module Arcane d’orchestration ShArc. Hérite de `ArcaneShArcObject`, `IAppServiceMng`, `ArcGeoSim::Delegate<IIterationInformation>`. |

### Méthodes clés

| Symbole | Type | Description |
|---------|------|-------------|
| `void build()` | point d’entrée | Prépare la connectivité du maillage, détecte le mode `ForecastCompute`, active les edges si nécessaire. |
| `void prepareInit()` | point d’entrée | Initialise les services d’application, récupère `IGeometryMng` et installe une politique de géométrie (`NoOptimizationGeometryPolicy`). |
| `void init()` | point d’entrée | Initialise la politique de pas de temps, enregistre `ITimeStepComputer` dans la `ITimeLine`, met à jour la géométrie, initialise post‑processing et validateurs. |
| `void continueInit()` | point d’entrée | Étape optionnelle pour compléter l’initialisation (actuellement simple rappel d’`initializeAppServiceMng()`). |
| `void endInit()` | point d’entrée | Sauvegarde optionnelle d’un état initial de post‑traitement (`save-init`). |
| `void beginTimeStep()` | point d’entrée | Vérifie que `ShArcModule` a bien été initialisé. |
| `void endTimeStep()` | point d’entrée | Gère la sortie de post‑traitement périodique et les conditions d’arrêt (itérations, timeline). |
| `void endSimulation()` | point d’entrée | Exécute la séquence de post‑traitement final puis appelle `_validate()`. |
| `void timeStepInformation(Integer nb_loop)` | méthode | Affiche un rapport temporel détaillé (T(n), T(n+1), Δt) pour le pas courant. |
| `VersionInfo versionInfo() const` | méthode | Retourne la version du module (`1.0.0`). |

### Validation et post‑traitement

| Symbole | Type | Description |
|---------|------|-------------|
| `void postProcessingInit()` | méthode privée | Initialise le `ArcGeoSimPostProcessingBase`, configure les writers et les variables à sortir, calcule `m_next_iteration`. |
| `void postProcessingExit()` | méthode privée | Sauvegarde finale des données, affichage des stats, fermeture des writers. |
| `void _validate()` | méthode privée | Parcourt les services `validator`, exécute `validate()` et arrête la boucle de calcul avec statut succès/échec. |

## Architecture

```text
┌───────────────────────────────────────────────┐
│                src/ShArc                     │
├───────────────────────────────────────────────┤
│   ShArcModule.cc / ShArcModule.h             │
│   ShArc.axl                                   │
│   config.xml                                  │
└───────────────────────────────────────────────┘
            │                 │
            ▼                 ▼
   Arcane (TimeLoop,      ArcGeoSim
   subDomain, mesh)       (services, post-processing,
                           time line, validators)
```

- `ShArc.axl` décrit les points d’entrée et les options de cas.
- `config.xml` déclare le module et ses dépendances (packages `arcane`, `alien`).
- `ShArcModule` fait le lien entre infrastructure Arcane et services ArcGeoSim.

## Dépendances

| Dépendance | Usage |
|------------|-------|
| `arcane` | Module de base (time loop, sous‑domaines, maillages, I/O). |
| `alien` (optionnel) | Gestion de systèmes linéaires (via la stack ArcGeoSim / autres modules). |
| `ArcGeoSim` | Services de temps (`ITimeLine`, `ITimeStepComputer`), géométrie, post‑traitement, validation, unités, expressions. |

## Configuration

Les options principales sont définies dans `ShArc.axl` :

| Option | Type | Description |
|--------|------|-------------|
| `units-system` | service `IUnitsSystem` | Système d’unités pour la simulation. |
| `time-manager` | service `ArcGeoSim::ITimeLine` | Gestionnaire de temps (nouvelle version). |
| `time-step-policy` | service `ArcGeoSim::TimeStep::IComputer` | Politique de calcul du pas de temps (par défaut `TimeStepEvolutionPolicy`). |
| `geometry` | service `IGeometryMng` | Gestionnaire de géométrie. |
| `expression-mng` | service `IExpressionMng` | Gestionnaire d’expressions. |
| `max-iteration` | entier | Nombre d’itérations maximum (0 pour illimité). |
| `post-processing` | bloc complexe | Configuration du post‑traitement (période, chemin, sauvegarde initiale, writers, variables). |
| `has-edges` | booléen | Indique si des propriétés/groupes sont associés aux edges (active la connectivité edges). |
| `validator` | services `IServiceValidator` | Validateurs appelés en fin de simulation. |

## Ressources

- `src/ShArc/ShArc.axl` — définition complète des options et points d’entrée du module.
- Documentation ArcGeoSim / Arcane — pour les concepts :
  - Time loops, services d’application, post‑processing.
  - `ArcGeoSimPostProcessingBase`, `ITimeLine`, `IServiceValidator`.
- Cas de test ShArc (sous `test/`) — exemples d’activation de `ShArc` avec différents scénarios physiques et options de post‑traitement.
