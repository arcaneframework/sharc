// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*------------------------------------------------------------------------------------*/
/* IfpMeshPartitioner.cc                                     (C) 2000-2007            */
/*                                                                                    */
/* Partitioneur de maillage utilisant la bibliotheque de partitionnement interne IFP  */
/*------------------------------------------------------------------------------------*/
/**
 * Variable d'environnement pour choisir l'algorithme de decoupage : PARTITIONER_CUT_TYPE
 *  'x' -> TypesIfpMeshPartitioner::X
 *  'y' -> TypesIfpMeshPartitioner::Y
 *  '1' -> TypesIfpMeshPartitioner::OneD
 *  '2' -> TypesIfpMeshPartitioner::TwoD
 *  'P' -> TypesIfpMeshPartitioner::TwoDMetis
 *  'M' -> TypesIfpMeshPartitioner::ThreeDMetis
 */
/*------------------------------------------------------------------------------------*/

#include <arcane/ArcaneVersion.h>
#include "arcane/utils/ArcanePrecomp.h"

#include "arcane/utils/PlatformUtils.h"
#include "arcane/utils/Convert.h"
#include "arcane/utils/NotImplementedException.h"

#include "arcane/ISubDomain.h"
#include "arcane/IParallelMng.h"
#include "arcane/ItemEnumerator.h"
#include "arcane/IMesh.h"
#include "arcane/IMeshSubMeshTransition.h"
#include "arcane/ItemGroup.h"
#include "arcane/Service.h"
#include "arcane/Timer.h"
#include "arcane/FactoryService.h"
#include "arcane/ItemPrinter.h"
#include "arcane/IItemFamily.h"
#include "arcane/MeshVariable.h"
#include "arcane/VariableBuildInfo.h"

#include "arcane/std/MeshPartitionerBase.h"
#include "TypesIfpMeshPartitioner.h"
#include "IfpMeshPartitioner_axl.h"

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Appli/AppService.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Mesh/Geometry/NoOptimizationGeometryPolicy.h"
#include "ArcGeoSim/Mesh/Utils/IXMTools/IXMItemConnections.h"

// Au cas ou on utilise mpich2 ou openmpi
#ifndef MPICH_SKIP_MPICXX
#define MPICH_SKIP_MPICXX
#endif
#ifndef OMPI_SKIP_MPICXX
#define OMPI_SKIP_MPICXX
#endif

#include "mpi.h"
#include "IFPPartitioner.h"
#include "metis.h"
#include "Utils/GraphUtils.h"

#include <unordered_map>
#include <vector>

//#define ARCANE_DEBUG_IFPPART

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*!
 * \brief Partitioneur de maillage utilisant la bibliotheque IFPMeshPartitioner.
 */


class IfpMeshPartitioner : public ArcaneIfpMeshPartitionerObject {
public:

  enum eIfpComputeTime {
    MTC_NoWeight,
    MTC_Global,
    MTC_TrackingOnly,
    MTC_Full,
    MTC_Full_OneWeight,
    MTC_Full_CellTimeOnly,
    MTC_Full_CellTime
  };

public:

  IfpMeshPartitioner(const ServiceBuildInfo &sbi);

  virtual ~IfpMeshPartitioner() {}

public:

  virtual void build();

public:

  void partitionMesh(bool initial_partition);

  void partitionMesh(bool initial_partition, Int32 nb_part);

private:
  enum LinkType {
    LinkTypeX = 0,
    LinkTypeY = 1,
    LinkTypeZ = 2
  };

  Integer _linkType(const IGeometryMng::Real3Variable &cell_center,
                    const Cell &cell, const Cell &opposite_cell);

  TypesIfpMeshPartitioner::eCutType m_cut_type;
  IItemFamily *m_cell_family;
  VariableCellReal m_cells_times;
  VariableCellReal m_old_cells_times;
  VariableCellReal m_cells_weight;
  bool m_is_first_loadbalance, m_is_parallel;
  std::unique_ptr<IGeometryPolicy> m_geometry_policy;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

IfpMeshPartitioner::IfpMeshPartitioner(const ServiceBuildInfo &sbi)
    : ArcaneIfpMeshPartitionerObject(sbi), m_cut_type(TypesIfpMeshPartitioner::OneD),
      m_cell_family(sbi.mesh()->cellFamily()),
      m_cells_times(VariableBuildInfo(sbi.mesh(), "MeshPartitionerCellsTimes",
                                      IVariable::PNoDump | IVariable::PExecutionDepend |
                                          IVariable::PNoRestore)),
      m_old_cells_times(VariableBuildInfo(sbi.mesh(), "MeshPartitionerOldCellsTimes",
                                          IVariable::PNoDump | IVariable::PExecutionDepend |
                                              IVariable::PNoRestore)),
      m_cells_weight(VariableBuildInfo(sbi.mesh(), "MeshPartitionerCellsWeight",
                                       IVariable::PNoDump | IVariable::PNoRestore)),
      m_is_first_loadbalance(true) {
  m_is_parallel = sbi.subDomain()->parallelMng()->isParallel();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

extern "C"
void F2C(ifppartitionerinit)(int *comm, int *needMPIInit);

void IfpMeshPartitioner::build() {
  //m_cut_type = options()->type();

  MPI_Comm comm = MPI_COMM_WORLD;
  int fcomm = MPI_Comm_c2f(comm);
  Integer needMpiInit = (m_is_parallel) ? 1 : 0;
  //needMpiInit=0;
  F2C(ifppartitionerinit)(&fcomm, &needMpiInit);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpMeshPartitioner::partitionMesh(bool initial_partition) {
  info() << "Equilibrage de charge avec Ifp-Athos partitioner\n";

  if (m_is_first_loadbalance) {
    //GG: ATTENTION: ne fonctionne pas avec les reprises
    m_old_cells_times.fill(0);
    m_is_first_loadbalance = false;
  }

  ISubDomain *sd = subDomain();
  Integer sub_domain_id = sd->subDomainId();
  IParallelMng *pm = sd->parallelMng();
#if (ARCANE_VERSION >= 20405)
  Integer nb_sub_domain = pm->commSize();
#else /*ARCANE_VERSION*/
  Integer nb_sub_domain = pm->nbSubDomain();
#endif /*ARCANE_VERSION*/
  Integer master_io_rank = pm->masterIORank();
  IMesh *mesh = this->mesh();
  Integer dim = mesh->dimension();

  if (mesh->hasTiedInterface())
    throw NotImplementedException(
        "IfpMeshPartitioner::partitionMesh",
        "can not partition mesh with tied interface");
  if (!initial_partition)
    throw NotImplementedException(
        "IfpMeshPartitioner::partitionMesh", "initial partition");

  CellGroup own_cells = mesh->ownCells();
  Integer nb_own_cell = own_cells.size();
  // Integer weight_dim = 1;

  ArcGeoSim::AppService<IGeometryMng> geometry_service;

  m_geometry_policy.reset(new NoOptimizationGeometryPolicy);

  // Politique de tol�rance pour le moment souple ...
  geometry_service->setPolicyTolerance(false);

  geometry_service->addItemGroupProperty(own_cells,
                                         IGeometryProperty::PCenter, IGeometryProperty::PVariable);
  geometry_service->update(m_geometry_policy.get());
  const IGeometryMng::Real3Variable &cell_center =
      geometry_service->getReal3VariableProperty(own_cells,
                                                 IGeometryProperty::PCenter);

  VariableItemInteger &cells_new_owner = PRIMARYMESH_CAST(this->mesh())->itemsNewOwner(IK_Cell);
  auto graph = ArcGeoSim::Mesh::GraphMng::graph(mesh,true) ;
  if (pm->commRank() == master_io_rank) // le master se charge du partitionnement local
  {

    std::vector<int> ifp_xadj;
    ifp_xadj.reserve(nb_own_cell + 1);

    // en maillage cartesien (seulement!), le nombre de mailles voisines a une maille donnee
    // est de l'ordre de 6 en 3D et de 4 en 2D
    // j'ai considere cette formule pour ne pas calculer le nb de voisins
    Integer nb_max_face_neighbour_cell = 2 * dim * nb_own_cell;
    /*
     {
     // Renumerote les mailles pour que chaque sous-domaine
     // ait des mailles de numero consecutifs
     ENUMERATE_CELL(i_item,own_cells){
     const Cell& item = *i_item;
     nb_max_face_neighbour_cell += item.nbFace();
     }
     } */

    std::vector<int> ifp_adjncy;
    ifp_adjncy.reserve(nb_max_face_neighbour_cell);

    SharedArray<int> cell_orientation;
    cell_orientation.reserve(nb_max_face_neighbour_cell);

    //std::string LinkTypeX="+X";
    //std::string LinkTypeY="+Y";
    //std::string LinkTypeZ="+Z";
    std::unordered_map<Int32, std::set<Int32>> f2f_cell_list;

#if 1
    ArcGeoSim::ItemConnectionMng item_connections(mesh, graph);
    const ArcGeoSim::ItemConnectionGroup& face_face_connections =
        item_connections.findConnectionGroup("FaceFaceConnections");
    if(!face_face_connections.null() && !face_face_connections.empty()) {
      ENUMERATE_FACEFACE_CONNECTION(iff, face_face_connections) {
        if (item_connections.first(iff).toFace().localId() != item_connections.second(iff).toFace().localId()) {
          f2f_cell_list[item_connections.first(iff).toFace().localId()].insert(
              item_connections.second(iff).toFace().cell(0).localId());

          f2f_cell_list[item_connections.second(iff).toFace().localId()].insert(
              item_connections.first(iff).toFace().cell(0).localId());
        }
      }
    }
#else
    const auto *links = mesh->itemFamily(eItemKind::IK_Link);
    const auto &face2face_link = links->findGroup("FaceFaceConnections");
    if (!face2face_link.empty()) {
      ENUMERATE_LINK(iff, face2face_link) {
        if (iff->dualNode(0).dualItem().toFace().localId() != iff->dualNode(1).dualItem().toFace().localId()) {

          f2f_cell_list[iff->dualNode(0).dualItem().toFace().localId()].insert(
              iff->dualNode(1).dualItem().toFace().cell(0).localId());

          f2f_cell_list[iff->dualNode(1).dualItem().toFace().localId()].insert(
              iff->dualNode(0).dualItem().toFace().cell(0).localId());
        }
      }
    }
#endif

#ifdef ARCANE_DEBUG_IFPPART
    pinfo() << "Debut de la construction du graphe de connection maille-maille";
#endif
    /** pour calculer le graphe d'orientation des mailles par rapport
     * aux directions X Y et Z. On calcule pour chaque couple de mailles
     * adjacentes (i,j), la longueur maximale de la projection du vecteur Vij sur
     * chaque direction de l'espace. L'orientation de (i,j) est la direction
     * de l'espace associee a la longueur maximale de la projection
     */

    /** Deux possibilites pour calculer les aretes du graphe qui doivent
     * correspondre aux mailles connectees entre elles. Soit on considere
     * les mailles connectees uniquement si elles ont une face commune, soit
     * on considere les mailles connectees si elles ont un noeud en commun.
     * La deuxieme solution est plus couteuse en memoire et temps de calcul
     * car elle donne plus d'aretes mais correspond a la maniere dont sont
     * calculees les mailles fantomes, donc on prend celle ci.
     * A terme, il faudra utiliser la methode qui est coherente avec celle
     * utilisee pour determiner les mailles fantomes.
     * ESSAIE AVEC 'true' pour limiter la memoire
     */
    bool use_only_face = true;
    if (use_only_face) {
      ENUMERATE_CELL(i_item, own_cells) {
        const Cell &cell = *i_item;
        ifp_xadj.push_back(ifp_adjncy.size());
        for (Integer z = 0, zs = cell.nbFace(); z < zs; ++z) {
          const Face &face = cell.face(z);
          if (face.nbCell() == 2) {
            const Cell &opposite_cell = (face.cell(0) == cell) ? face.cell(1) : face.cell(0);

            cell_orientation.add(_linkType(cell_center, cell, opposite_cell));
            ifp_adjncy.push_back(static_cast<int> (opposite_cell.localId()));

          }
#if 1
          else if (face.nbCell() == 1 && !f2f_cell_list.empty()) {
            const auto lid2cell = mesh->cellFamily()->view();

            const auto i_cell_list = f2f_cell_list.find(face.localId());
            if (i_cell_list != f2f_cell_list.cend()) {
              for (const auto &cell_lid : i_cell_list->second) {
                const auto &add_cell = lid2cell[cell_lid].toCell();
                if (add_cell.localId() != cell.localId()) {
                  cell_orientation.add(_linkType(cell_center, cell, add_cell));
                  ifp_adjncy.push_back(static_cast<int> (add_cell.localId()));
                }
              }
            }
          }
#endif
        }
      }
      ifp_xadj.push_back(ifp_adjncy.size());
    } else {
#if (ARCANE_VERSION >= 30003)
      SharedArray<Int64> neighbour_cells(mesh->itemFamily(IK_Cell)->maxLocalId()+1);
#else
      SharedArray<Int64> neighbour_cells(mesh->itemFamily(IK_Cell)->variableMaxSize());
#endif
      neighbour_cells.fill(NULL_ITEM_ID);
      ENUMERATE_CELL(i_item, own_cells) {
        const Cell &item = *i_item;
        Int64 item_uid = item.uniqueId().asInt64();
        Integer nb_neighbour = 0;
        // On ne se rajoute pas a notre liste de maille connectee
        neighbour_cells[item.localId()] = item_uid;
        ifp_xadj.push_back(ifp_adjncy.size());
        for (Integer z = 0, zs = item.nbNode(); z < zs; ++z) {
          const Node &node = item.node(z);
          for (Integer zc = 0, zcs = node.nbCell(); zc < zcs; ++zc) {
            const Cell &node_cell = node.cell(zc);
            Integer node_cell_local_id = node_cell.localId();
            // Regarde si on n'est pas dans la liste et si on n'y est pas,
            // s'y rajoute
            if (neighbour_cells[node_cell_local_id] != item_uid) {
              neighbour_cells[node_cell_local_id] = item_uid;
              ifp_adjncy.push_back(static_cast<int> (node_cell.uniqueId()));
              ++nb_neighbour;
            }
          }
        }
      }
      ifp_xadj.push_back(ifp_adjncy.size());
    }
    // declaration des tableaux de sorties de IFPPartitioner
#ifdef ARCANE_DEBUG_IFPPART
    pinfo() << "Fin de la construction du graphe de connection maille-maille";
#endif

    Integer nb_unknowns_cell = 1; // number of unknowns per cell (NCOU)
    Integer cut_type_size = 1; // la taille de LbTyp (nbb)

    String cut_type_flag = platform::getEnvironmentVariable("PARTITIONER_CUT_TYPE");
    char cut_type[] = "1"; // dans le fortran LBTyp. ATT: cette variable passera dans les options
    if (!cut_type_flag.null()) {
      info() << "PARTITIONER_CUT_TYPE ENVIRONNEMENT FLAG= " << cut_type_flag;
      cut_type[0] = cut_type_flag.localstr()[0];
    }
    info() << "PARTITIONER_CUT_TYPE = " << cut_type;
    /*
    switch (m_cut_type)
    {
      case TypesIfpMeshPartitioner::X:
        cut_type[0] = 'x';
        break;
      case TypesIfpMeshPartitioner::Y:
        cut_type[0] = 'y';
        break;
        case TypesIfpMeshPartitioner::OneD :
        cut_type[0] = '1';
        break;
        case TypesIfpMeshPartitioner::TwoD :
        cut_type[0] = '2';
        break;
      case TypesIfpMeshPartitioner::TwoDMetis:
        cut_type[0] = 'P';
        break;
      case TypesIfpMeshPartitioner::ThreeDMetis:
        cut_type[0] = 'M';
        break;
    }*/
    Integer ifp_adjncy_size = ifp_adjncy.size();

    std::vector<int> parts(nb_own_cell);
    // Appel a l'interface (F2C) du partitionneur IFP
    F2C(ifppartitioner)(&nb_unknowns_cell,
                        &nb_own_cell,
                        &nb_sub_domain,
                        ifp_xadj.data(),
                        &ifp_adjncy_size,
                        ifp_adjncy.data(),
                        &cut_type_size,
                        cut_type,
                        cell_orientation.unguardedBasePointer(),
                        parts.data());

    {
      String npart_node = platform::getEnvironmentVariable("PARTITIONER_NPART_NODE");
      Integer nb_new_owner = 0;
      Integer index = 0;

      if(npart_node.empty()) {
        Integer index = 0;
        ENUMERATE_CELL(i_item, own_cells) {
          const Cell &item = *i_item;
          Integer new_owner = parts[index] - 1;
          if (new_owner != item.owner())
            ++nb_new_owner;
          ++index;
          cells_new_owner[item] = new_owner;
        }
      }
      else {
        int nb_sub_domain_per_node = std::stoi(npart_node.localstr());
        pinfo() << "Nb subdomains per node from PARTITIONER_NPART_NODE :" <<  nb_sub_domain_per_node << '\n';

        std::vector<std::map<int, int>> part_adjcny(nb_sub_domain);

        for (unsigned i = 0; i < ifp_xadj.size() - 1; ++i) {
          for (int j = ifp_xadj[i] - 1; j < ifp_xadj[i + 1] - 1; ++j) { // Fortran indexing
            // ifp_adjncy and parts are Fortran indexed
            if (parts[ifp_adjncy[j] - 1] != parts[i]) {
              part_adjcny[parts[i] - 1][parts[ifp_adjncy[j] - 1] - 1]++;
            }
          }
        }

        std::vector<int> metis_adjcny_list(nb_sub_domain + 1);

        metis_adjcny_list[0] = 0;
        for (int i = 1; i < nb_sub_domain + 1; ++i) {
          metis_adjcny_list[i] = metis_adjcny_list[i - 1] + part_adjcny[i - 1].size();
        }

        std::vector<int> metis_adjcny(metis_adjcny_list[nb_sub_domain]);

        for (int i = 0; i < nb_sub_domain; ++i) {
          for (const auto &neigh : part_adjcny[i]) {
            if (i != neigh.first) {
              metis_adjcny[metis_adjcny_list[i]++] = neigh.first;
            }
          }
        }

        metis_adjcny_list[0] = 0;
        for (int i = 1; i < nb_sub_domain + 1; ++i) {
          metis_adjcny_list[i] = metis_adjcny_list[i - 1] + part_adjcny[i - 1].size();
        }

        std::vector<int> metis_part(nb_sub_domain);

        int nb_part = nb_sub_domain / nb_sub_domain_per_node;
        int val = 0;
        int ncons = 1;

        idx_t options[METIS_NOPTIONS];
        METIS_SetDefaultOptions(options);
        options[METIS_OPTION_CONTIG] = 1;
        options[METIS_OPTION_UFACTOR] = (((nb_sub_domain_per_node + 1.0) / nb_sub_domain_per_node - 1) * 1000 - 1) / 10;

        METIS_PartGraphKway(&nb_sub_domain, &ncons, metis_adjcny_list.data(), metis_adjcny.data(),
                            nullptr, nullptr, nullptr,
                            &nb_part,
                            nullptr, nullptr, options,
                            &val, metis_part.data());

        std::vector<int> part_size(nb_part, 0);
        std::vector<int> rank_in_part(nb_sub_domain);
        int min_part_size = std::numeric_limits<int>::max();
        int max_part_size = 0;
        double avg_part_size = 0;

        for (int i = 0; i < nb_sub_domain; ++i) {
          rank_in_part[i] = part_size[metis_part[i]]++;
        }
        for (int i = 0; i < nb_part; ++i) {
          avg_part_size += part_size[i];
          if (part_size[i] < min_part_size) {
            min_part_size = part_size[i];
          }
          if (part_size[i] > max_part_size) {
            max_part_size = part_size[i];
          }
        }
        avg_part_size /= nb_part;

        pinfo() << "Sub domains partition size: " << '\n'
                << "  avg = " << avg_part_size << '\n'
                << "  min = " << min_part_size << '\n'
                << "  max = " << max_part_size << '\n';

        ENUMERATE_CELL(i_item, own_cells) {
          const Cell &item = *i_item;
          Integer new_owner = rank_in_part[parts[index] - 1] + metis_part[parts[index] - 1] * nb_sub_domain_per_node;
          if (new_owner != item.owner())
            ++nb_new_owner;
          ++index;
          cells_new_owner[item] = new_owner;
        }
      }

      pinfo() << "Proc " << sub_domain_id
              << " nombre de mailles changeant de domaine: " << nb_new_owner
              << " reste=" << (index - nb_new_owner);
    }
  }
  cells_new_owner.synchronize();
  changeOwnersFromCells();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void IfpMeshPartitioner::partitionMesh(bool initial_partition, Int32 nb_part) {
  ISubDomain *sd = subDomain();
  IParallelMng *pm = sd->parallelMng();
  Integer nb_procs = pm->commSize();
  if (nb_procs != nb_part)
    fatal() << "Can only partition for npart == nprocs number";
  else
    return partitionMesh(initial_partition);
}

Integer IfpMeshPartitioner::_linkType(const IGeometryMng::Real3Variable &cell_center,
                                      const Cell &cell, const Cell &opposite_cell) {

  Real3 ij = cell_center[opposite_cell] - cell_center[cell];
  Real dd = math::abs(ij.x);
  Integer linkType = LinkTypeX;

  if (math::abs(ij.y) > dd) {
    dd = math::abs(ij.y);
    linkType = LinkTypeY;
  }
  if (math::abs(ij.z) > dd) {
    dd = math::abs(ij.z);
    linkType = LinkTypeZ;
  }

  return linkType;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
ARCANE_REGISTER_SUB_DOMAIN_FACTORY(IfpMeshPartitioner, IMeshPartitioner, Ifpart);
ARCANE_REGISTER_SERVICE_IFPMESHPARTITIONER(Ifpart, IfpMeshPartitioner);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
