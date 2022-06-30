// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
/* Author : desrozis at Wed Aug  1 14:30:22 2012
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "../ISystemBuilder.h"

#include "UserSystem_axl.h"

#include "ArcRes/Builders/Factory.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class UserSystemService 
    : public ArcaneUserSystemObject
      {
      public:

  UserSystemService(const Arcane::ServiceBuildInfo & sbi)
      : ArcaneUserSystemObject(sbi) {}

  ~UserSystemService() {}

      public:

  ArcRes::System buildSystem();

      };

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ArcRes::System
UserSystemService::
buildSystem()
{
  using namespace ArcRes;

  info() << "UserSystem buildSystem";

  Factory factory;

  FluidSubSystemBuilder fluid = factory.newFluidSubSystem(options()->fluidSystem().name());
  SolidSubSystemBuilder solid = factory.newSolidSubSystem(options()->solidSystem().name());

  SystemBuilder system = factory.newSystem(options()->name(), fluid, solid);

  // Pour les reactions
  std::map<Arcane::String, SpeciesBuilder> species_builders;
  // Pour les composants par defauts
  Arcane::StringUniqueArray species_names;

  info() << "Build MultiPhase System";
  // fluide
  const Arcane::Integer nb_fluid_phases = options()->fluidSystem().fluidPhase.size();
  for(Arcane::Integer i = 0; i < nb_fluid_phases; ++i) {
    FluidPhaseBuilder phase = factory.newFluidPhase(options()->fluidSystem().fluidPhase[i].name());
    fluid << phase;
    const Arcane::Integer nb_species = options()->fluidSystem().fluidPhase[i].species.size();
    for(Arcane::Integer j = 0; j < nb_species; ++j) {
      const Arcane::String name = options()->fluidSystem().fluidPhase[i].species[j];
      SpeciesBuilder species = factory.newSpecies(name);
      phase << species;
      if(species_builders.find(name) != species_builders.end())
        fatal() << "species '" << name << "' multiples defined";
      species_builders[name] = species;
      species_names.add(name);
    }
  }

  // solide
  const Arcane::Integer nb_solid_phases = options()->solidSystem().solidPhase.size();
  for(Arcane::Integer i = 0; i < nb_solid_phases; ++i) {
    SolidPhaseBuilder phase = factory.newSolidPhase(options()->solidSystem().solidPhase[i].name());
    solid << phase;
    const Arcane::Integer nb_species = options()->solidSystem().solidPhase[i].species.size();
    for(Arcane::Integer j = 0; j < nb_species; ++j) {
      const Arcane::String name = options()->solidSystem().solidPhase[i].species[j];
      SpeciesBuilder species = factory.newSpecies(name);
      phase << species;
      if(species_builders.find(name) != species_builders.end())
        fatal() << "species '" << name << "' multiples defined";
      species_builders[name] = species;
      species_names.add(name);
    }
  }

  // Le systeme utilisateur

  return system.entity();
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
//using namespace Geoxim;
ARCANE_REGISTER_SERVICE_USERSYSTEM(UserSystem,UserSystemService); 
