// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef MESHUTILS_H_
#define MESHUTILS_H_

#include "ArcGeoSim/Utils/Utils.h"
#include <arcane/VariableTypes.h>
#include <arcane/ISubDomain.h>
#include <arcane/IVariable.h>
#include <arcane/ItemGroup.h>
#include <arcane/IMesh.h>
#include <arcane/IItemFamily.h>

#include "ArcGeoSim/Utils/MeshVarExpr.h"

BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_MESH_NAMESPACE

#ifndef USE_ARCANE_V3
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


#ifndef USE_ARCANE_V3
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
