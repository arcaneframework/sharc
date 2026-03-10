// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "CAWF/PreCICE/DynamicMeshMng/IDynamicMeshMng.h"

#include "DynamicMeshMng_axl.h"

#include <arcane/IMesh.h>
#include <arcane/IPrimaryMesh.h>
#include <arcane/IMeshReader.h>
#include <arcane/ServiceFinder.h>
#include <arcane/ServiceBuilder.h>
#include <arcane/IMainFactory.h>
#include <arcane/IMeshPartitioner.h>
#include <arcane/IMeshModifier.h>
#include <arcane/IMeshUtilities.h>
#include <arcane/IItemFamily.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class DynamicMeshMngService
: public ArcaneDynamicMeshMngObject
{
public:

  DynamicMeshMngService(const Arcane::ServiceBuildInfo & sbi)
      : ArcaneDynamicMeshMngObject(sbi) {}

  ~DynamicMeshMngService() {}

public:
  void init() ;
  void updateNewEvent() ;
private:
  Arcane::String m_mesh_name;
  Arcane::String m_mesh_format;
  Arcane::IMeshReader* m_mesh_reader = nullptr;
  Arcane::Integer m_current_event_iter = 0 ;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
using namespace Arcane;

void DynamicMeshMngService::init()
{
  m_mesh_name          = options()->mesh() ;
  m_mesh_format        = options()->format();
  m_current_event_iter = options()->firstEventId();

  Arcane::ServiceBuilder<IMeshReader> sb(subDomain());
  String reader_name("Undefined") ;
  if(m_mesh_format == "vt2")
    reader_name = "IfpVtkMeshIO" ;
  if(m_mesh_format == "vtk")
    reader_name = "VtkLegacyMeshReader," ;
  if(m_mesh_format == "msh")
    reader_name = "MshMeshReader" ;
  if(reader_name != "Undefined")
  {
    m_mesh_reader = sb.createInstance(reader_name);
    bool read_ok = false;
    if (m_mesh_reader)
    {
      if (m_mesh_reader->allowExtension(m_mesh_format))
        read_ok = true;
      if (!read_ok)
        fatal() << "No mesh reader found for format : "<<m_mesh_format;
    }
  }
}

void DynamicMeshMngService::updateNewEvent()
{
  ++m_current_event_iter;
  String current_mesh_name = String::format("{0}-{1}Event","DynamicMesh",m_current_event_iter) ;
  Arcane::IPrimaryMesh* new_mesh = subDomain()->mainFactory()->createMesh(subDomain(),current_mesh_name);
  String filename = String::format("{0}{1}.{2}",m_mesh_name,m_current_event_iter,m_mesh_format);
  info() << "READ Mesh FILE : "<<filename<<" For Event : "<<m_current_event_iter;
  m_mesh_reader->readMeshFromFile(new_mesh,XmlNode(),filename,Arcane::String(),true);
  info()<<"Evolutive Mesh INFO - Event : "<<m_current_event_iter;
  info()<<"MESH INFO["<<m_current_event_iter<<"] NEW TOTAL NB CELL = "<<new_mesh->allCells().size();
  CellGroup new_cells = new_mesh->cellFamily()->findGroup("IXMEvolutiveMesh_newCells");
  if(not new_cells.null())
    info()<<"MESH INFO["<<m_current_event_iter<<"] NEW CELLS GROUP SIZE = "<<new_cells.size();
  NodeGroup new_nodes = new_mesh->nodeFamily()->findGroup("IXMEvolutiveMesh_newNodes");
  if(not new_nodes.null())
    info()<<"MESH INFO["<<m_current_event_iter<<"] NEW NODES GROUP SIZE = "<<new_nodes.size();
  NodeGroup updated_nodes = new_mesh->nodeFamily()->findGroup("IXMEvolutiveMesh_updatedNodes");
  if(not updated_nodes.null())
    info()<<"MESH INFO["<<m_current_event_iter<<"] UPDATE NODE GROUP SIZE = "<<updated_nodes.size();

  auto mesh       = subDomain()->mesh();
  auto coords     = mesh->nodesCoordinates() ;
  auto new_coords = new_mesh->nodesCoordinates() ;
  UniqueArray<Int64> new_node_uids;
  UniqueArray<Int64> new_cell_uids;
  if(not new_cells.null() && new_cells.size()>0)
  {
    auto mesh = subDomain()->mesh();
    auto modifier = mesh->modifier() ;
    {
      Integer nb_new_nodes = new_nodes.size() ;
      new_node_uids.reserve(nb_new_nodes) ;
      ENUMERATE_NODE(inode,new_nodes)
      {
        new_node_uids.add(inode->uniqueId()) ;
      }
      modifier->addNodes(new_node_uids) ;
    }

    {
      Integer max_nb_nodes_per_cell = 10 ;
      ItemTypeMng* itm = mesh->itemTypeMng();

      Integer nb_new_cells = new_cells.size() ;
      UniqueArray<Int64> cell_connectivity;
      cell_connectivity.reserve(max_nb_nodes_per_cell) ;
      UniqueArray<Int64> cells_infos;
      cell_connectivity.reserve(nb_new_cells*(2+max_nb_nodes_per_cell)) ;
      ENUMERATE_CELL(icell,new_cells)
      {
        new_cell_uids.add(icell->uniqueId()) ;
        Int32 current_cell_nb_node = icell->nbNode();
        auto type_id = icell->type() ;
        cell_connectivity.clear() ;
        for(auto inode : icell->nodes())
          cell_connectivity.add(inode->uniqueId()) ;
        cells_infos.add(type_id);
        cells_infos.add(icell->uniqueId());
        cells_infos.addRange(cell_connectivity);
      }
      UniqueArray<Int32> cell_lids;
      modifier->addCells(nb_new_cells,cells_infos,cell_lids) ;
    }
    modifier->endUpdate() ;
  }
  /*
  if(not new_nodes.null() && new_nodes.size()>0)
  {
    UniqueArray<Int32> new_node_lids(new_node_uids.size());
    mesh->nodeFamily()->itemsUniqueIdToLocalId(new_node_lids,new_node_uids);
    auto nodes_view = mesh->nodeFamily()->view(new_node_lids) ;
    {
      Int32 node_index = 0;
      ENUMERATE_NODE(inode,new_nodes)
      {
        coords[nodes_view[node_index++].toNode()] = new_coords[*inode] ;
      }
    }
  }*/

  //if(not updated_nodes.null() && updated_nodes.size()>0)
  {
    UniqueArray<Int64> updated_node_uids;
    updated_node_uids.reserve(new_mesh->allNodes().size()) ;
    ENUMERATE_NODE(inode,new_mesh->allNodes())
      updated_node_uids.add(inode->uniqueId()) ;
    UniqueArray<Int32> updated_node_lids(updated_node_uids.size());
    mesh->nodeFamily()->itemsUniqueIdToLocalId(updated_node_lids,updated_node_uids);
    auto nodes_view = mesh->nodeFamily()->view(updated_node_lids) ;
    {
      Int32 node_index = 0;
      ENUMERATE_NODE(inode,new_mesh->allNodes())
      {
        coords[nodes_view[node_index++].toNode()] = new_coords[*inode] ;
      }
    }
  }
  if(new_cell_uids.size()>0)
  {
    UniqueArray<Int32> new_cell_lids(new_cell_uids.size());
    mesh->cellFamily()->itemsUniqueIdToLocalId(new_cell_lids,new_cell_uids);
    mesh->cellFamily()->createGroup("NewEventLayer",new_cell_lids,true) ;

    IMeshUtilities* mu = mesh->utilities();

    auto update_face_group = [&](String group_name) {
      FaceGroup boundary = new_mesh->faceFamily()->findGroup(group_name) ;
      Integer nb_face = boundary.size();
      UniqueArray<Integer> faces_nb_node;
      faces_nb_node.reserve(nb_face) ;
      UniqueArray<Int64> faces_connectivity;
      faces_connectivity.reserve(4*nb_face);
      ENUMERATE_FACE(iface,boundary)
      {
        faces_nb_node.add(iface->nbNode()) ;
        ENUMERATE_NODE(inode,iface->nodes())
        {
          faces_connectivity.add(inode->uniqueId()) ;
        }
      }
      UniqueArray<Integer> faces_lid(nb_face);
      mu->localIdsFromConnectivity(IK_Face,faces_nb_node,faces_connectivity,faces_lid);
      mesh->faceFamily()->createGroup("TopBoundary",faces_lid,true) ;
      FaceGroup top_boundary = mesh->faceFamily()->findGroup(group_name) ;
      info()<<"BOUNDARY["<<group_name<<"] GROUP SIZE : "<<boundary.size();
    } ;

    info()<<" EVENT INDEX : "<<m_current_event_iter;
    update_face_group("TopBoundary") ;
    update_face_group("BottomBoundary") ;
    update_face_group("FrontBoundary") ;
    update_face_group("BackBoundary") ;
    update_face_group("LeftBoundary") ;
    update_face_group("RightBoundary") ;
  }
  else
  {
    CellGroup new_event = mesh->cellFamily()->findGroup("NewEventLayer") ;
    if(not new_event.empty())
      new_event.clear();
  }
}


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
//using namespace Geoxim;
ARCANE_REGISTER_SERVICE_DYNAMICMESHMNG(DynamicMeshMng,DynamicMeshMngService);
