// -*- C++ -*-
#ifndef ARCGEOSIM_MESH_MAPPING_PARTICLETOMESHMAPPINGIMPL_LOCALPARTICLEMAPPING_H
#define ARCGEOSIM_MESH_MAPPING_PARTICLETOMESHMAPPINGIMPL_LOCALPARTICLEMAPPING_H


#include <arcane/IMesh.h>

#include <arcane/utils/ITraceMng.h>

#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshTools.h"
#include "ArcGeoSim/Mesh/Utils/MappingTools/MeshCellAccessor.h"
#include "ArcGeoSim/Mesh/AccessorToolsMng/IMeshAccessorToolsMng.h"

using namespace Arcane;

class LocalParticleMapping
{
public:

  /** Constructeur de la classe */
  LocalParticleMapping(ArcGeoSim::IMeshAccessorToolsMng* mng);

  /** Destructeur de la classe */
  virtual ~LocalParticleMapping()
  {}

public:

  /** A method */
  void upscale(ParticleVectorView particle_list);

  void downscale(ParticleVectorView particle_list);

  void setTraceManager(ITraceMng* trace_mng)
  {
    m_debug = true;
    m_trace_mng = trace_mng;
  }

private:
  IMesh*            m_mesh  = nullptr;
  MeshTools*        m_tools = nullptr;
  MeshCellAccessor* m_acc   = nullptr;

  Real m_epsilon = 0 ;
  bool m_debug   = false ;
  ITraceMng* m_trace_mng = nullptr;
};

#endif /* ARCGEOSIM_MESH_MAPPING_PARTICLETOMESHMAPPINGIMPL_LOCALPARTICLEMAPPING_H */
