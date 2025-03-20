#include "GlobalParticleMapping.h"

#include <arcane/mesh/ParticleFamily.h>


GlobalParticleMapping::GlobalParticleMapping(ArcGeoSim::IMeshAccessorToolsMng* mng)
{
  m_debug = false;

  m_acc   = mng->getMeshAccessor();
  m_mesh  = m_acc->getMesh() ;
  m_tools = mng->getMeshTools();

  OrthonormalBoundingBox bbox = m_acc->getBoundingBox();

  m_grid = mng->getRegularGrid();
  m_x_min = bbox.getXMin();
  m_x_max = bbox.getXMax();
  m_y_min = bbox.getYMin();
  m_y_max = bbox.getYMax();
  m_z_min = bbox.getZMin();
  m_z_max = bbox.getZMax();

  m_tab.resize(3);

  m_epsilon = math::pow(10., -15.) * (bbox.getXMax() - bbox.getXMin()) * (bbox.getYMax() - bbox.getYMin())
      * (bbox.getZMax() - bbox.getZMin());

}

/*---------------------------------------------------------------------------*/
void GlobalParticleMapping::run(ParticleVectorView particle_list)
{
  IItemFamily* myFam = m_mesh->findItemFamily(IK_Particle, "Particle", true);

  VariableParticleReal3 particle_coords_variable(VariableBuildInfo(m_mesh, "ParticleCoord", "Particle"));

  UniqueArray<Integer> result;

  ENUMERATE_PARTICLE(ipart,particle_list)
  {
    getAssociatedCells(particle_coords_variable[ipart], result);

    if (!result.empty())
    {
      myFam->toParticleFamily()->setParticleCell(*ipart, m_mesh->meshItemInternalList()->cells.item(result[0]));
    }
    else
    {
      Cell null_cell ;
      myFam->toParticleFamily()->setParticleCell(*ipart,null_cell ) ;
    }
    if (m_debug)
    {
      const Particle & myPart = *ipart;

      m_trace_mng->info() << "Particle " << ipart->localId()<<" coord:"<<particle_coords_variable[ipart];
      if (myPart.hasCell() && !result.empty())
      {
        m_trace_mng->info() << "\t(particle.cell=" << myPart.cell().localId() << ") result="<<result[0];
      }
      else
      {
        m_trace_mng->info() << "\t(particle.cell=VIDE)";
      }
    }

    if (m_debug)
    {
      m_trace_mng->info() << "\tassociated to cells " << result;
    }
  }
}

/*---------------------------------------------------------------------------*/
void GlobalParticleMapping::getAssociatedCells(const Real3& center, UniqueArray<Integer>& list)
{
  list.clear();

  bool to_add = false;

  Cell cell;

  // Regarde si le points est dans la boite englobante du maillage
  if ((center.x < m_x_min) || (center.x > m_x_max) || (center.y < m_y_min)
      || (center.y > m_y_max || (center.z < m_z_min) || (center.z > m_z_max)))
  {
    return;
  }

  m_grid->getIJK(center, m_tab);

  Integer index = m_grid->getIJK(m_tab[0], m_tab[1], m_tab[2]);

  UniqueArray<Integer> m_ite;
  m_grid->getElementsFromCell(index, m_ite);

  for (IntegerSharedArray::iterator target = m_ite.begin(); target != m_ite.end(); target++)
  {
    to_add = false;

    cell = m_mesh->meshItemInternalList()->cells.item(*target);

    // On regarde si le point est dans cette cellule
    if(cell.isActive())
    	to_add = m_tools->isInCell(cell, center, m_epsilon);

    if (to_add)
    {
      list.add(*target);
    }
  }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
