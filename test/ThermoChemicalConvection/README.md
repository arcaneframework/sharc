# Tests ThermoChemicalConvection

> 🧠 Documentation enrichie par IA — basée sur l'analyse des fichiers de cas et du module ThermoChemicalConvection

## 📋 Table des matières

- [Description](#description)
- [Objectifs](#objectifs)
- [Utilisation](#utilisation)
- [Ressources](#ressources)

## Description

Le répertoire `test/ThermoChemicalConvection/` contient un cas de test pour le module `ThermoChemicalConvection`, qui résout un système couplé **pression–concentration–température** en milieu poreux.

Le cas met en jeu :

- un maillage 3D `mesh-10x1x10.vt2`,
- un fichier de cas `use-case-thermal.arc` configurant :
  - le système ArcRes (phases fluide/solide, propriétés thermiques),
  - les services de conditions initiales, lois physiques et conditions aux limites,
- un fichier `ShArc.config` pour l'infrastructure ShArc/Arcane (time-loop, modules).

## Objectifs

- Valider la construction de l'accumulation et des flux (advection, diffusion, conduction) définis dans `ThermoChemicalConvectionModule`.
- Vérifier la cohérence thermique (capacité et conductivité des phases fluide et solide) sur un cas simple.
- Servir de cas de régression pour les évolutions du module thermo‑chimique.

## Utilisation

Depuis le répertoire de build :

```bash
ctest -R ThermoChemicalConvection
```

Ou directement :

```bash
./ShArc.exe test/ThermoChemicalConvection/use-case-thermal.arc
```

Adapter les options MPI et solveurs en fonction de la configuration de votre build.

## Ressources

- `src/TwoPhaseFlowSimulation/README.md` — sections consacrées au module `ThermoChemicalConvection`.
- `src/TwoPhaseFlowSimulation/ThermoChemicalConvection*.axl` — descripteurs du module.
- Documentation ArcRes — définition des propriétés thermiques et des sous‑systèmes fluide/solide.
