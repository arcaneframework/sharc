// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#include <arcane/mesh/ParticleFamily.h>
// Interface du service 
#include "ArcGeoSim/Mesh/AccessorToolsMng/IMeshAccessorToolsMng.h"

#include "IParticleMapping.h"
#include "ParticleMapping_axl.h"

#include "ArcGeoSim/Mesh/Mapping/ParticleToMeshMappingImpl/LocalParticleMapping.h"
#include "ArcGeoSim/Mesh/Mapping/ParticleToMeshMappingImpl/GlobalParticleMapping.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ParticleMappingService: public ArcaneParticleMappingObject
{
public:

  /** Constructeur de la classe */
  ParticleMappingService(const Arcane::ServiceBuildInfo & sbi)
  : ArcaneParticleMappingObject(sbi)
  , m_family_name("Particle")
  {
  }

  /** Destructeur de la classe */
  ~ParticleMappingService()
  {
  }

public:
  //!
  void init(ArcGeoSim::IMeshAccessorToolsMng* mesh_accessor_tools_mng);

  //! Creation d'une famille de particle TODO a deplacer d'ici
  IItemFamily* createParticleFamily(ConstArrayView<Int64> particle_uids,
                                    ConstArrayView<Real3> particle_coords);

  //! Creation d'une famille de particle TODO a deplacer d'ici
  IItemFamily* createParticleFamily(ConstArrayView<Int64> particle_uids,
                                    ConstArrayView<Integer> cell_lids,
                                    ConstArrayView<Real3> particle_coords);
  //!
  void globalParticleMapping(ParticleVectorView particle_list);

  //!
  void localParticleMappingUpscale(ParticleVectorView particle_list);

  //!
  void localParticleMappingDownscale(ParticleVectorView particle_list);

private:
  //! maillage courant
  ArcGeoSim::IMeshAccessorToolsMng* m_mesh_accessor_tools_mng = nullptr ;
  //! methode locale de mapping
  std::unique_ptr<LocalParticleMapping> m_local_mapping ;
  //!
  std::unique_ptr<VariableParticleReal3> m_particle_coords ;

  String m_family_name ;

  bool m_initialized = false ;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void ParticleMappingService::init(ArcGeoSim::IMeshAccessorToolsMng* mesh_accessor_tools_mng)
{
  m_mesh_accessor_tools_mng = mesh_accessor_tools_mng ;
  m_family_name = options()->familyName() ;
  m_initialized = true ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
IItemFamily* ParticleMappingService::createParticleFamily(ConstArrayView<Int64> particle_uids,
                                                          ConstArrayView<Real3> particle_coords)
{
  if(! m_initialized) fatal()<<"ParticleMappingService not initalized";
    IItemFamily* i_item_family = mesh()->findItemFamily(IK_Particle,m_family_name, true);

    Int32SharedArray particle_lids(particle_uids.size());

    ParticleVectorView particles = i_item_family->toParticleFamily()->addParticles(particle_uids, particle_lids);

    i_item_family->endUpdate();

    m_particle_coords.reset(new VariableParticleReal3(VariableBuildInfo(mesh(), "ParticleCoord",m_family_name)));
    m_particle_coords->setUsed(true);

    for (Integer i = 0, is = particle_lids.size(); i < is; i++)
    {
    	const Particle& particle = particles[i].internal();
    	(*m_particle_coords)[particle] = particle_coords[i];
    }

    return i_item_family;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
IItemFamily*
ParticleMappingService::createParticleFamily( ConstArrayView<Int64> particle_uids,
                                              ConstArrayView<Integer> cell_lids,
                                              ConstArrayView<Real3> particle_coords)
{
  if(! m_initialized) fatal()<<"ParticleMappingService not initalized";

  IItemFamily* i_item_family = mesh()->findItemFamily(IK_Particle,m_family_name, true);

  Int32SharedArray particle_lids(particle_uids.size());

  ParticleVectorView particles = i_item_family->toParticleFamily()->addParticles(particle_uids,cell_lids,particle_lids);

  i_item_family->endUpdate();

  m_particle_coords.reset(new VariableParticleReal3(VariableBuildInfo(mesh(), "ParticleCoord", m_family_name)));
  m_particle_coords->setUsed(true);

    for (Integer i = 0, is = particle_lids.size(); i < is; i++)
    {
    	const Particle& particle = particles[i].internal();
    	(*m_particle_coords)[particle] = particle_coords[i];
    }
    return i_item_family;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void ParticleMappingService::globalParticleMapping(ParticleVectorView particle_list)
{
  if(! m_initialized) fatal()<<"ParticleMappingService not initalized";
  GlobalParticleMapping my_mapping(m_mesh_accessor_tools_mng);
  if(options()->verbose())
    my_mapping.setTraceManager(traceMng());
  my_mapping.run(particle_list);
}

/*---------------------------------------------------------------------------*/

// Deraffinement, on prend le parent.
void ParticleMappingService::localParticleMappingUpscale(ParticleVectorView particle_list)
{
  if(! m_initialized) fatal()<<"ParticleMappingService not initalized";
  if(m_local_mapping.get()==nullptr)
    m_local_mapping.reset(new  LocalParticleMapping(m_mesh_accessor_tools_mng));
  m_local_mapping->upscale(particle_list);
}

/*---------------------------------------------------------------------------*/

// Raffinement, on prend un enfant.
void ParticleMappingService::localParticleMappingDownscale(ParticleVectorView particle_list)
{
  if(! m_initialized) fatal()<<"ParticleMappingService not initalized";
  if(m_local_mapping.get()==nullptr)
    m_local_mapping.reset(new  LocalParticleMapping(m_mesh_accessor_tools_mng));
  m_local_mapping->downscale(particle_list);
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_PARTICLEMAPPING(ParticleMapping, ParticleMappingService);
