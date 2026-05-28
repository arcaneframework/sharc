// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#include <arcane/ArcaneVersion.h>
#include <arcane/utils/ArcanePrecomp.h>

#include <arcane/utils/Iostream.h>
#include <arcane/utils/StdHeader.h>
#include <arcane/utils/HashTableMap.h>
#include <arcane/utils/ValueConvert.h>
#include <arcane/utils/ScopedPtr.h>
#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/String.h>
#include <arcane/utils/StringBuilder.h>
#include <arcane/utils/IOException.h>
#include <arcane/utils/Collection.h>
#include <arcane/utils/Enumerator.h>
#include <arcane/utils/OStringStream.h>
#include <arcane/ArcaneVersion.h>
#include <arcane/FactoryService.h>
#include <arcane/IMeshReader.h>
#include <arcane/ISubDomain.h>
#include <arcane/IMeshSubMeshTransition.h>
#include <arcane/IItemFamily.h>
#include <arcane/Item.h>
#include <arcane/ItemEnumerator.h>
#include <arcane/VariableTypes.h>
#include <arcane/IParallelMng.h>

#include <arcane/IIOMng.h>
#include <arcane/IXmlDocumentHolder.h>
#include <arcane/XmlNodeList.h>
#include <arcane/XmlNode.h>

#include <arcane/IMeshUtilities.h>
#include <arcane/IMeshWriter.h>
#include <arcane/BasicService.h>
#include <arcane/ServiceBuilder.h>

#include "arcane/utils/PlatformUtils.h"
#include "arcane/utils/Convert.h"
#include "arcane/utils/NotImplementedException.h"

#include <arcane/utils/UserDataList.h>
#include <arcane/utils/IUserData.h>
#include <arcane/utils/AutoDestroyUserData.h>

#include "ArcGeoSim/Utils/Utils.h"
#include "ouranos/config.h"
#include "ouranos/kernel/info.hpp"
#include "ouranos/mesh/mesh.hpp"
#include "ouranos/kernel/ouranos.hpp"

using namespace Arcane;

class MeshbMeshReader
: public BasicService
, public IMeshReader
{
public:
  MeshbMeshReader(const ServiceBuildInfo& sbi);
public:
virtual bool allowExtension(const String& str);
virtual IMeshReader::eReturnType readMeshFromFile(IPrimaryMesh* mesh,
                                     const XmlNode& mesh_element,
                                     const String& file_name,
                                     const String& dir_name,
                                     bool use_internal_partition);

};

MeshbMeshReader::MeshbMeshReader(const ServiceBuildInfo& sbi)
: Arcane::BasicService(sbi)
{
}

bool MeshbMeshReader::allowExtension(const String& str)
{
  return str== "meshb";
}

IMeshReader::eReturnType
MeshbMeshReader::
readMeshFromFile(IPrimaryMesh* mesh,
                 const XmlNode& mesh_element,
                 const String& file_name,
                 const String& dir_name,
                 bool use_internal_partition)
{
  Arcane::Trace::Setter setter(traceMng(),"IXMMeshReader");

  info() << "=== READING Meshb MESH ===";

  info () << "Testing ouranos linking " << Ouranos::Kernel::version();
  auto rns = std::make_shared<Ouranos::Kernel::Ouranos>();
  Ouranos::Mesh::Mesh<3, Ouranos::Kernel::Block> msh(rns);
  msh.read(file_name.localstr());

  const auto& vertices = msh.ver();
  Integer nb_nodes = vertices.nbr();

  info() << "Ouranos version: " << Ouranos::Kernel::version();
  info() << "Mesh read: " << nb_nodes << " nodes, "
         << msh.tet().nbr() << " tetrahedra " ;

  // Arcane mesh
  mesh->setDimension(3);

  // Get tetras
  const auto& tets = msh.tet();
  Integer nb_tets = tets.nbr();

  // Build cells_infos array
  SharedArray<Int64> cells_infos;
  cells_infos.reserve(nb_tets * 6);

  // Adding tetra
  for (Integer i = 0; i < nb_tets; ++i) {
      cells_infos.add(IT_Tetraedron4);
      cells_infos.add(i);
      for (Integer j = 0; j < 4; ++j) {
          cells_infos.add(tets.con()[i + 1][j]);
      }
   }


  // Allocate cells
  mesh->allocateCells(nb_tets, cells_infos, false);
  mesh->endAllocate();

  info() << "Arcane mesh created: " << mesh->nbNode() << " nodes, "
         << mesh->nbCell() << " cells";

  // Assign coords to nodes
  const auto& coords = vertices.crd();
  VariableNodeReal3& nodes_coord = mesh->nodesCoordinates();

  ENUMERATE_NODE (inode, mesh->allNodes()) {
      const Node& node = *inode;
      Int64 uid = node.uniqueId().asInt64();
      rns_real_t x = coords[uid][0];
      rns_real_t y = coords[uid][1];
      rns_real_t z = coords[uid][2];
      nodes_coord[inode] = Real3(x, y, z);
  }

// Mesh into vtk for debug
IMeshWriter* writer = Arcane::ServiceBuilder<IMeshWriter>::createInstance(
    mesh->subDomain(), "VtkLegacyMeshWriter"
);
if (writer) {
    writer->writeMeshToFile(mesh, file_name + ".vtk");
    info() << "Mesh saved to " << file_name << ".vtk";
} else {
    info() << "Writer not found";
}
  // auto refined_msh = Ouranos::Mesh::refine(rns, msh);

  return RTOk;
};

ARCANE_REGISTER_SUB_DOMAIN_FACTORY(MeshbMeshReader,IMeshReader,MeshbMeshReaderService);