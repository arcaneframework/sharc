// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_MAPPING_IPARTICLEMAPPING_H
#define ARCGEOSIM_MESH_MAPPING_IPARTICLEMAPPING_H


#include <arcane/IMesh.h>
//#include <arcane/utils/Array.h>
#include <arcane/Item.h>

class IParticleMapping
{
public:

  /** Destructeur de la classe */
  virtual ~IParticleMapping()
  {
  }

public:

  virtual void init(ArcGeoSim::IMeshAccessorToolsMng* mesh_accessor_tools_mng) = 0;

  // TODO a deplacer d'ici
  virtual Arcane::IItemFamily* createParticleFamily(Arcane::ConstArrayView<Arcane::Int64> particle_uids,
      Arcane::ConstArrayView<Arcane::Real3> particle_coords) = 0;
  // TODO a deplacer d'ici
  virtual Arcane::IItemFamily* createParticleFamily(Arcane::ConstArrayView<Arcane::Int64> particle_uids,
      Arcane::ConstArrayView<Arcane::Integer> cell_lids,
      Arcane::ConstArrayView<Arcane::Real3> particle_coords) = 0;

  virtual void globalParticleMapping(Arcane::ParticleVectorView particle_list) = 0;

  virtual void localParticleMappingUpscale(Arcane::ParticleVectorView particle_list) = 0;

  virtual void localParticleMappingDownscale(Arcane::ParticleVectorView particle_list) = 0;
};

#endif /* ARCGEOSIM_MESH_MAPPING_IPARTICLEMAPPING_H */
