#include "LocalParticleMapping.h"

#include <arcane/mesh/ParticleFamily.h>


/*---------------------------------------------------------------------------*/
LocalParticleMapping::LocalParticleMapping(ArcGeoSim::IMeshAccessorToolsMng* mng)
{
  m_debug = false;

  m_acc   = mng->getMeshAccessor();
  m_mesh  = m_acc->getMesh() ;
  m_tools = mng->getMeshTools();

  OrthonormalBoundingBox bbox = m_acc->getBoundingBox();

  m_epsilon = math::pow(10., -15.) * (bbox.getXMax() - bbox.getXMin()) * (bbox.getYMax() - bbox.getYMin())
      * (bbox.getZMax() - bbox.getZMin());
}

/*---------------------------------------------------------------------------*/
void LocalParticleMapping::upscale(ParticleVectorView particle_list)
{
  IItemFamily* particle_family = m_mesh->findItemFamily(IK_Particle, "Particle", true);

  ENUMERATE_PARTICLE(iparticle,particle_list)
  {
    const Cell& cell = iparticle->cell();

    if ((!cell.null()) && ( cell.internal()->flags() & ItemInternal::II_Coarsen))
    {
    	particle_family->toParticleFamily()->setParticleCell(*iparticle, cell.hParent().toCell());
    }
  }
}

/*---------------------------------------------------------------------------*/
void LocalParticleMapping::downscale(ParticleVectorView particle_list)
{
  IItemFamily* particle_family = m_mesh->findItemFamily(IK_Particle, "Particle", true);

  VariableParticleReal3 particle_coords_variable(VariableBuildInfo(m_mesh, "ParticleCoord", "Particle"));

  Real3 particle_coord;

  ENUMERATE_PARTICLE(ipart,particle_list)
  {
	  particle_coord= particle_coords_variable[ipart];

    const Cell& cell = ipart->cell();

    if (!cell.null())
    {
      for (Integer i = 0, is = cell.nbHChildren(); i < is ; i++)
      {
        const Cell& child = cell.hChild(i).toCell();
        if (m_tools->isInCell(child, particle_coord, m_epsilon))
        {
          particle_family->toParticleFamily()->setParticleCell(*ipart, child);
          break;
        }
      }
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
