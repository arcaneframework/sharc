// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_PARTICLEREADER_H
#define ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_PARTICLEREADER_H



#include <arcane/IXmlDocumentHolder.h>
#include <arcane/IIOMng.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/XmlNodeList.h>
#include <arcane/IXmlDocumentHolder.h>
#include <arcane/mesh/ParticleFamily.h>


#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IIXMMeshReader.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMTools.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMMeshReaderTools.h"
#include "ArcGeoSim/Utils/ParallelUtils.h"

BEGIN_ARCGEOSIM_NAMESPACE
using namespace Arcane;

class ParticleReader
{
public:
  
  /** Constructeur de la classe */
  ParticleReader(IMesh* mesh,
                 Arcane::XmlNode mesh_data,
                 Arcane::IParallelMng* parallel_mng,
                 Arcane::ITraceMng* trace_mng,
                 const Arcane::String& directory_name);
  
  /** Destructeur de la classe */
  virtual ~ParticleReader() {}
  
public:

  /** A method */
  void collectiveRead();

private:

  IMesh* m_mesh;
  Arcane::XmlNode m_mesh_data;
  Arcane::IParallelMng* m_parallel_mng;
  Arcane::ITraceMng* m_trace_mng;
  VariableParticleReal3* m_particle_coords;
  Arcane::String m_directory_name;

  void _readParticleInfos(
      const XmlNodeList& particle_list_node,
      const String& block_name,
      ItemTypeMng * typeMng);
};

END_ARCGEOSIM_NAMESPACE

#endif /* ARCGEOSIM_ARCGEOSIM_MESH_UTILS_IXMTOOLS_PARTICLEREADER_H */
