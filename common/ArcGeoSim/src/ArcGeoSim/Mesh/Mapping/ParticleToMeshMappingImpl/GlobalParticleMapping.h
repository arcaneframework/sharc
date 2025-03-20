// -*- C++ -*-
#ifndef ARCGEOSIM_MESH_MAPPING_PARTICLETOMESHMAPPINGIMPL_GLOBALPARTICLEMAPPING_H
#define ARCGEOSIM_MESH_MAPPING_PARTICLETOMESHMAPPINGIMPL_GLOBALPARTICLEMAPPING_H

#include <arcane/IMesh.h>
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshCellAccessor.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/RegularGrid.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshTools.h"
#include "ArcGeoSim/Mesh/AccessorToolsMng/IMeshAccessorToolsMng.h"

#include <arcane/utils/ITraceMng.h>

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Class GlobalParticleMapping Global Particle Mapping on Cells
 *
 * This is imported from OpenFlow's ElementAssociation and ElementAssociationByLocalisationInUnstructuredCells classes.
 */
class GlobalParticleMapping
{
public:

  /** Constructeur de la classe */
  GlobalParticleMapping(ArcGeoSim::IMeshAccessorToolsMng* mng);

  /** Destructeur de la classe */
  ~GlobalParticleMapping()
  {}

public:
  /** run / relocalisation globale */
  void run(ParticleVectorView particle_list);

  /** debug purpose - set the trace manager */
  void setTraceManager(ITraceMng* trace_mng)
  {
    m_debug = true;
    m_trace_mng = trace_mng;
  }

private:
  MeshCellAccessor* m_acc   = nullptr;
  RegularGrid*      m_grid  = nullptr;
  MeshTools*        m_tools = nullptr;
  IMesh*            m_mesh  = nullptr;

  /** debug flag for trace purpose */
  bool m_debug = false;
  /** trace manager */
  ITraceMng* m_trace_mng = nullptr;

  Real m_x_min, m_x_max;
  Real m_y_min, m_y_max;
  Real m_z_min, m_z_max;

  IntegerSharedArray m_tab;

  Real m_epsilon = 0;

private:
  void getAssociatedCells(const Real3& center, UniqueArray<Integer>& list);

};

#endif /* ARCGEOSIM_MESH_MAPPING_PARTICLETOMESHMAPPINGIMPL_GLOBALPARTICLEMAPPING_H */
