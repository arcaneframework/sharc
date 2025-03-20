// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Utils/Utils.h"

#include "ArcGeoSim/Mesh/Utils/MeshUtils.h"

#if (ARCANE_VERSION >= 30003)
#include "arcane/utils/AutoDestroyUserData.h"
#include "arcane/utils/IUserDataList.h"
#endif

using namespace Arcane ;

BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_MESH_NAMESPACE

#if (ARCANE_VERSION >= 30003)

Arcane::IGraph2*
GraphMng::graph(Arcane::IMesh * mesh,bool create_if_required)
{
  const char* name = "DefaultDualGraph";
  IUserDataList* udlist = mesh->userDataList();

  IUserData* ud = udlist->data(name,true);
  if (ud){
      AutoDestroyUserData<Arcane::IGraph2>* adud = dynamic_cast<AutoDestroyUserData<Arcane::IGraph2>*>(ud);
      if (!adud)
        ARCANE_FATAL("Can not cast to IGraph2*");
      return adud->data();
  }
  else {
    if (!create_if_required)
      return nullptr;
   auto graph = Arcane::mesh::GraphBuilder::createGraph(mesh) ;
   udlist->setData(name,new AutoDestroyUserData<Arcane::IGraph2>(graph));
   return graph;
  }
}

const Arcane::IGraphConnectivity*
GraphMng::graphConnectivity(Arcane::IMesh * mesh)
{
  auto g = graph(mesh) ;
  if(g)
    return g->connectivity() ;
  else
    return nullptr ;
}

template<>
Cell dualToItem<Cell>(Arcane::IGraphConnectivity const* connectivity,ItemEnumeratorT<DualNode>& inode)
{
  return connectivity->dualItem(*inode).toCell() ;
}

template<>
Face dualToItem<Face>(Arcane::IGraphConnectivity const* connectivity,ItemEnumeratorT<DualNode>& inode)
{
  return connectivity->dualItem(*inode).toFace() ;
}

template<>
Particle dualToItem<Particle>(Arcane::IGraphConnectivity const* connectivity,ItemEnumeratorT<DualNode>& inode)
{
  return connectivity->dualItem(*inode).toParticle() ;
}
#else
template<>
Cell dualToItem<Cell>(ItemEnumeratorT<DualNode>& inode) 
{
  return (*inode).dualItem().toCell() ;
}

template<>
Face dualToItem<Face>(ItemEnumeratorT<DualNode>& inode) 
{
  return (*inode).dualItem().toFace() ;
}

template<>
Particle dualToItem<Particle>(ItemEnumeratorT<DualNode>& inode)
{
  return (*inode).dualItem().toParticle() ;
}
#endif

Edge getEdgeFromNodesUnique(Node node,Int64ConstArrayView edge_nodes_unique_id)
{
   Integer n = edge_nodes_unique_id.size();

   for( Integer i=0, s=node.nbEdge(); i<s; ++i ){
     Edge e(node.edge(i));
     Integer en = e.nbNode();
     if (en==n){
       bool same_edge = true;
       for( Integer zz=0; zz<n; ++zz )
         if (e.node(zz).uniqueId()!=edge_nodes_unique_id[zz]){
           same_edge = false;
           break;
         }
       if (same_edge)
         return e;
     }
   }

   return Edge();
}


END_MESH_NAMESPACE

END_ARCGEOSIM_NAMESPACE
