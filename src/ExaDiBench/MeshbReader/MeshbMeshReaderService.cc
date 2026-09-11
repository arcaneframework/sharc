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

// Add every elements from a specific types (tetra, hexa, ...) to cells_infos.
template <typename ElementT>
Integer appendBlockElements(
    const ElementT& elts,
    Int64 arcane_type,
    Integer nb_nodes_per_elt,
    SharedArray<Int64>& cells_infos,
    Integer uid_offset)
{
  Integer nb = elts.nbr();
  cells_infos.reserve(cells_infos.size() + nb * (nb_nodes_per_elt + 2));
  for (Integer i = 0; i < nb; ++i) {
    cells_infos.add(arcane_type);
    cells_infos.add(uid_offset + i);
    for (Integer j = 0; j < nb_nodes_per_elt; ++j)
      cells_infos.add(elts.con()[i + 1][j]);
  }
  return nb;
}

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
 
  using metis_t = Ouranos::Mesh::Partitioning::Metis;
  

  Integer sid = subDomain()->subDomainId();
  IParallelMng* pm = subDomain()->parallelMng();
  auto pcom = pm->communicator();
  MPI_Comm mpi_com = (pcom.isValid())  ? static_cast<const MPI_Comm>(pcom) : MPI_COMM_WORLD;
  
  bool master = sid == 0 ;
  
  if (use_internal_partition)
  {
      Integer nb_cells = 0;
      Integer dim = 0;
      SharedArray<Int64> cells_infos;
      SharedArray<Real3> node_coords_master;

      if (master)
      {
          info() << "=== READING Meshb MESH ===";
          info() << "Testing ouranos linking " << Ouranos::Kernel::version();
          MPI_Comm seq_com = MPI_COMM_SELF;
          Ouranos::Kernel::MPI::Com com(seq_com);
          auto rns = std::make_shared<Ouranos::Kernel::Ouranos>(com);
          dim = Ouranos::Mesh::read_mesh_dimension(rns, file_name.localstr());
          info() << "Mesh dimension : " << dim;

          if (dim == 3)
          {
              Ouranos::Mesh::Mesh<3, Ouranos::Kernel::Block> msh(rns);
              msh.read(file_name.localstr());

              const auto& vertices = msh.ver();
              Integer nb_nodes = vertices.nbr();
              info() << "Mesh read: " << nb_nodes << " nodes";

              Integer uid_offset = 0;
              Integer nb_tets = appendBlockElements(msh.tet(), IT_Tetraedron4, 4, cells_infos, uid_offset);
              uid_offset += nb_tets;
              Integer nb_hex = appendBlockElements(msh.hex(), IT_Hexaedron8, 8, cells_infos, uid_offset);
              uid_offset += nb_hex;
              nb_cells = uid_offset;
              info() << "Mesh read: " << nb_tets << " tetrahedra, " << nb_hex << " hexahedra";

              const auto& coords = vertices.crd();
              node_coords_master.resize(nb_nodes + 1);
              for (Integer i = 1; i <= nb_nodes; ++i)
                  node_coords_master[i] = Real3(coords[i][0], coords[i][1], coords[i][2]);
          }
          else if (dim == 2)
          {
              Ouranos::Mesh::Mesh<2, Ouranos::Kernel::Block> msh(rns);
              msh.read(file_name.localstr());

              const auto& vertices = msh.ver();
              Integer nb_nodes = vertices.nbr();
              info() << "Mesh read: " << nb_nodes << " nodes";

              Integer uid_offset = 0;
              Integer nb_tri = appendBlockElements(msh.tri(), IT_Triangle3, 3, cells_infos, uid_offset);
              uid_offset += nb_tri;
              nb_cells = uid_offset;
              info() << "Mesh read: " << nb_tri << " triangles";

              const auto& coords = vertices.crd();
              node_coords_master.resize(nb_nodes + 1);
              for (Integer i = 1; i <= nb_nodes; ++i)
                  node_coords_master[i] = Real3(coords[i][0], coords[i][1], 0.0);
          }
          else
          {
              fatal() << "Dimension " << dim << " not supported by Ouranos";
          }
      }

      pm->broadcast(ArrayView<Integer>(1, &dim), 0);

      mesh->setDimension(dim);
      mesh->allocateCells(nb_cells, cells_infos, false);
      mesh->endAllocate();

      {
          VariableNodeReal3& nodes_coord = mesh->nodesCoordinates();
          ENUMERATE_NODE (inode, mesh->allNodes()) {
              const Node& node = *inode;
              Int64 uid = node.uniqueId().asInt64();
              nodes_coord[inode] = node_coords_master[uid];
          }
      }
  }
else
{
  // Every mpi process reads the mesh -> Ouranos read the mesh and partition all the data 
  //Collective routine
  info() << "Ouranos versions: " << Ouranos::Kernel::version();

  Ouranos::Kernel::MPI::Com com(mpi_com);
  auto rns = std::make_shared<Ouranos::Kernel::Ouranos>(com);
  int dim = Ouranos::Mesh::read_mesh_dimension(rns,file_name.localstr()) ;

  switch(dim)
  {
    case 3:
    {
      Ouranos::Mesh::Mesh<3, Ouranos::Kernel::Block> msh(rns);
      msh.read(file_name.localstr());
      int nb_ghost_layer = 0 ; 
      auto part_mesh = msh.to_part<metis_t>(nb_ghost_layer) ; 

      auto& vertices = part_mesh.ver();
      Integer nb_nodes = vertices.nbr();

      info() << "Ouranos version: " << Ouranos::Kernel::version();
      info() << "Mesh read: " << nb_nodes << " nodes, "
            << part_mesh.tet().nbr() << " tetrahedra " ;

      // Arcane mesh
      mesh->setDimension(3);

      // Get tetras
      auto& tets = part_mesh.tet();
      Integer nb_tets = tets.nbr();

      info() << "nb_tets local (with duplicated) = " << nb_tets;

      auto vertex_part = vertices.location();
      // Ghost/None flag per local vertex (a vertex always has exactly one owner)
      auto& vertex_flags = vertices.flg();
      const auto& tet_gids = tets.location()->gid();

      SharedArray<Int64> cells_infos;
      cells_infos.reserve(nb_tets * 6);

      // Number of tets this rank actually keeps, after filtering
      Integer nb_owned_tets = 0;
      for (Integer i = 0; i < nb_tets; ++i) {
          std::array<rns_int_t, 4> node_gids;
          for (Integer j = 0; j < 4; ++j) {
              rns_int_t lid = tets.con()(i + 1)[j];
              node_gids[j] = vertex_part->lid_to_gid(lid);
          }

          // Only the owner of the smallest-gid vertex keeps the tet
          rns_int_t min_gid = *std::min_element(node_gids.begin(), node_gids.end());
          bool keep = !Ouranos::Kernel::Flag::is(vertex_flags[min_gid][0], Ouranos::Kernel::Flag::Ghost);

          if (!keep)
              // Another rank owns (or will own) this tet
              continue;

          cells_infos.add(IT_Tetraedron4);
          cells_infos.add(tet_gids[i]);
          for (Integer j = 0; j < 4; ++j)
              cells_infos.add(node_gids[j]);
          ++nb_owned_tets;
      }

      info() << "Tetrahedra: " << nb_tets << " local (with ghosts), " << nb_owned_tets << " owned";

      mesh->setDimension(3);
      // nb_owned_tets (filtered), not nb_tets (with duplicates)
      mesh->allocateCells(nb_owned_tets, cells_infos, false);
      mesh->endAllocate();

      auto& coords = vertices.crd();
      VariableNodeReal3& nodes_coord = mesh->nodesCoordinates();

      ENUMERATE_NODE (inode, mesh->allNodes()) {
          const Node& node = *inode;
          Int64 uid = node.uniqueId().asInt64();
          nodes_coord[inode] = Real3(coords[uid][0], coords[uid][1], coords[uid][2]);
      }
    }
    break ;
    case 2:
    {
      fatal()<<"NOT YET IMPLEMENTED" ;
    }
    break ; 
    default:
    {
      fatal()<<"Dimension" << dim << "not supported by Ouranos" ;
    }
  }
}


// // Mesh into vtk for debug
// IMeshWriter* writer = Arcane::ServiceBuilder<IMeshWriter>::createInstance(
//     mesh->subDomain(), "VtkLegacyMeshWriter"
// );
// if (writer) {
//     writer->writeMeshToFile(mesh, file_name + ".vtk");
//     info() << "Mesh saved to " << file_name << ".vtk";
// } else {
//     info() << "Writer not found";
// }
//   // auto refined_msh = Ouranos::Mesh::refine(rns, msh);

  return RTOk;
};

ARCANE_REGISTER_SUB_DOMAIN_FACTORY(MeshbMeshReader,IMeshReader,MeshbMeshReaderService);