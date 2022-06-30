// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Utils/Utils.h"

#include "ArcGeoSim/Mesh/Utils/MeshUtils.h"

using namespace Arcane ;

BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_MESH_NAMESPACE

#ifndef USE_ARCANE_V3
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
