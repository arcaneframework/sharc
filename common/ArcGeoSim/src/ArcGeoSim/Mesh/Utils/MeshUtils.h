// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef MESHUTILS_H_
#define MESHUTILS_H_

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Mesh/Utils/GraphUtils.h"
#include <arcane/VariableTypes.h>
#include <arcane/ISubDomain.h>
#include <arcane/IVariable.h>
#include <arcane/ItemGroup.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>

#include "ArcGeoSim/Utils/MeshVarExpr.h"

BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_MESH_NAMESPACE

#if (ARCANE_VERSION >= 30003)
template<typename Item>
Item dualToItem(Arcane::IGraphConnectivity const* connectivity,ItemEnumeratorT<DualNode>& inode) ;
#else
template<typename Item>
Item dualToItem(ItemEnumeratorT<DualNode>& inode) ;
#endif
/*
template<>
Cell dualToItem<Cell>(ItemEnumeratorT<DualNode>& inode) ;

template<>
Face dualToItem<Face>(ItemEnumeratorT<DualNode>& inode) ;*/

template<typename DataType,
         typename ItemType>
void assign(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
            const Real& value,
            const Arcane::ItemGroupT<ItemType>& group)
{
  for( ItemEnumeratorT<ItemType> item((group).enumerator()); 
       item.hasNext(); ++item )
  {
    var[item] = value ;
  }
} ;

template<typename DataType,
         typename ItemType, 
         typename Expr>
void assign(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
            const Expr& expr,
            const Arcane::ItemGroupT<ItemType>& group)
{
  for( ItemEnumeratorT<ItemType> item((group).enumerator()); 
       item.hasNext(); ++item )
  {
    var[item] = expr[item] ;
  }
} ;


#if (ARCANE_VERSION >= 30003)
template<typename DataType,
typename ItemType,
typename Expr>
void assignFromDualExpr(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
               const Expr& expr,
               const Arcane::ItemGroupT<DualNode>& group)
{
  if(group.null())
    return ;
  auto graph = ArcGeoSim::Mesh::GraphMng::graph(group.mesh()) ;
  auto connectivity = graph->connectivity() ;
  ENUMERATE_DUALNODE(inode,group)
  {
    const ItemType& item = dualToItem<ItemType>(connectivity,inode) ;
    var[item] = expr[inode] ;
  }
} ;

template<typename DataType,
typename ItemType,
typename Expr>
void assignForDualGroup(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
               const Expr& expr,
               const Arcane::ItemGroupT<DualNode>& group)
{
  if(group.null())
    return ;
  auto graph = ArcGeoSim::Mesh::GraphMng::graph(group.mesh()) ;
  auto connectivity = graph->connectivity() ;
  ENUMERATE_DUALNODE(inode,group)
  {
    const ItemType& item = dualToItem<ItemType>(connectivity,inode) ;
    var[item] = expr[item] ;
  }
} ;

#else
template<typename DataType,
         typename ItemType, 
         typename Expr>
void assignFromDualExpr(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
                        const Expr& expr,
                        const Arcane::ItemGroupT<DualNode>& group)
{
  ENUMERATE_DUALNODE(inode,group)
  {
    const ItemType& item = dualToItem<ItemType>(inode) ;
    var[item] = expr[inode] ;
  }
} ;

template<typename DataType,
         typename ItemType, 
         typename Expr>
void assignForDualGroup(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
                        const Expr& expr,
                        const Arcane::ItemGroupT<DualNode>& group)
{
  ENUMERATE_DUALNODE(inode,group)
  {
    const ItemType& item = dualToItem<ItemType>(inode) ;
    var[item] = expr[item] ;
  }
} ;
#endif
Edge getEdgeFromNodesUnique(Node node,Int64ConstArrayView edge_nodes_unique_id);


END_MESH_NAMESPACE

END_ARCGEOSIM_NAMESPACE
#endif /*MESHUTILS_H_*/
