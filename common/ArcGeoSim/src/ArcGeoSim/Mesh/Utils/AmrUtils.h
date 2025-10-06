// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * MeshUtils.h
 *
 *  Created on: Nov 11, 2009
 *      Author: gratienj
 */

#ifndef ARCGEOSIM_MESH_AMRUTILS_H_
#define ARCGEOSIM_MESH_AMRUTILS_H_
#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include <arcane/MathUtils.h>
#include <arcane/ItemGroup.h>

/**
 * template class to manage a key and a part of a Numerical Domain
 */
BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_MESH_NAMESPACE

namespace MeshDomain {

  inline void copy(ArrayView<Real> val, Real3 point) {
    val[0] = point.x ;
    val[1] = point.y ;
    val[2] = point.z ;
  }

  inline void copy(ArrayView<Real> val, Real2 point) {
    val[0] = point.x ;
    val[1] = point.y ;
  }


  inline Real3 toReal3(const Real3& c) {
    return c ;
  }

  inline Real3 toReal3(const Real2& c) {
    return Real3(c.x,c.y,0.) ;
  }

  inline Real2 toReal2(const Real3& c) {
    return Real2(c.x,c.y) ;
  }

  inline Real2 toReal2(const Real2& c) {
    return c ;
  }

  inline void setIdentity(Array<Real2>& matrix)
  {
    matrix[0] = Real2(1.,0.) ;
    matrix[1] = Real2(0.,1.) ;
  }

  inline void setIdentity(Array<Real3>& matrix)
  {
    matrix[0] = Real3(1.,0.,0.) ;
    matrix[1] = Real3(0.,1.,0.) ;
    matrix[2] = Real3(0.,0.,1.) ;
  }
}

inline Real meshMinSize(CellGroup const& cells,VariableItemReal3 const& cell_center,VariableItemReal3 const& face_center)
{

  Real mesh_size = 1.e10 ;
  ENUMERATE_CELL(icell,cells)
  {
    const Cell& cell = *icell ;
    for(FaceEnumerator iface(cell.faces());iface();++iface)
    {
      Real3 cf = face_center[*iface] - cell_center[icell] ;
      Real df = Arcane::math::sqrt(Arcane::math::scaMul(cf,cf)) ;
      ARCANE_ASSERT((df>0.),("Degenerated Cell")) ;
      mesh_size = Arcane::math::min(mesh_size,df) ;
    }
  }
  return mesh_size ;
}

template<typename DomainType>
Real meshSize(DomainType const& domain)
{
  typename DomainType::PointVariableType const& cell_center = domain.template getCenters<Cell>() ;
  typename DomainType::PointVariableType const& face_center = domain.template getCenters<Face>() ;

  Real mesh_size = 0 ;
  ENUMERATE_CELL(icell,domain.cells())
  {
    const Cell& cell = *icell ;
    Real cell_size = 0. ;
    for(FaceEnumerator iface(cell.faces());iface();++iface)
    {
      typename DomainType::point_type cf = face_center[*iface] - cell_center[icell] ;
      Real df = Arcane::math::sqrt(Arcane::math::scaMul(cf,cf)) ;
      cell_size = Arcane::math::max(cell_size,df) ;
    }
    mesh_size = Arcane::math::max(mesh_size,2*cell_size) ;
  }
  return mesh_size ;
}


template<typename DomainT,typename ItemT>
bool isBoundary(DomainT const& domain,
                ItemT const& item)
{
  return domain.isBoundary(item) ;
}


template<typename DomainT,typename ItemT>
bool isBoundaryOutside(DomainT const& domain,
                       ItemT const& item)
{
  return domain.isBoundaryOutside(item) ;
}



template<typename DomainT>
bool isInternalBoundary(const DomainT& domain,
                        Face const& item)
{
  return domain.isInternalBoundary(item) ;
}

inline bool isActive(Face const& face)
{
  return (face.isSubDomainBoundary()&&face.boundaryCell().isActive()) ||
         (face.backCell().isActive()&&face.frontCell().isActive()) ;
}

inline Integer nbChildFaces(Face const& face)
{
  Integer nb = 0 ;
  if(isActive(face))
  {
    return 1;
  }
  else
  {
    if(face.isSubDomainBoundary())
    {
       return 1 ;
    }
    else
    {
      Cell back_cell = face.backCell() ;
      Cell front_cell = face.frontCell() ;
      if(back_cell.isActive())
      {
        for(Integer i=0, nc=front_cell.nbHChildren();i<nc;++i)
        {
          Cell c = front_cell.hChild(i).toCell() ;
          for(FaceEnumerator iface(c.faces());iface();++iface)
          {
            if((iface->backCell().localId()==back_cell.localId())||(iface->frontCell().localId()==back_cell.localId()))
              ++nb ;
          }
        }
      }
      if(front_cell.isActive())
      {
        for(Integer i=0, nc=back_cell.nbHChildren();i<nc;++i)
        {
          Cell c = back_cell.hChild(i).toCell() ;
          for(FaceEnumerator iface(c.faces());iface();++iface)
          {
            if((iface->backCell().localId()==front_cell.localId())||(iface->frontCell().localId()==front_cell.localId()))
              ++nb ;
          }
        }
      }
    }
  }
  return nb ;
}

inline void addChildFaces(Face const& face,ItemVectorT<Face>& vector)
{
  if(isActive(face))
  {
    vector.addItem(face) ;
    return ;
  }
  else
  {
    if(face.isSubDomainBoundary())
    {
      vector.addItem(face) ;
       return ;
    }
    else
    {
      Cell back_cell = face.backCell() ;
      Cell front_cell = face.frontCell() ;
      if(back_cell.isActive())
      {
        for(Integer i=0, nc=front_cell.nbHChildren();i<nc;++i)
        {
          Cell c = front_cell.hChild(i).toCell() ;
          for(FaceEnumerator iface(c.faces());iface();++iface)
          {
            if((iface->backCell().localId()==back_cell.localId())||(iface->frontCell().localId()==back_cell.localId()))
              vector.addItem(*iface) ;
          }
        }
      }
      if(front_cell.isActive())
      {
        for(Integer i=0, nc=back_cell.nbHChildren();i<nc;++i)
        {
          Cell c = back_cell.hChild(i).toCell() ;
          for(FaceEnumerator iface(c.faces());iface();++iface)
          {
            if((iface->backCell().localId()==front_cell.localId())||(iface->frontCell().localId()==front_cell.localId()))
              vector.addItem(*iface) ;
          }
        }
      }
    }
  }
}

inline void childFaces(Face const& face,ItemVectorT<Face>& vector)
{
  vector.clear() ;
  addChildFaces(face,vector) ;
}

inline Integer nbActiveFaces(Cell const& cell)
{
  Integer nb = 0 ;
  for(FaceEnumerator iface(cell.faces());iface();++iface)
    nb += nbChildFaces(*iface) ;
  return nb ;
}

inline void activeFaces(Cell const& cell,ItemVectorT<Face>& vector)
{
  for(FaceEnumerator iface(cell.faces());iface();++iface)
    addChildFaces(*iface,vector) ;
}
inline void activeFaces(Node const& node,ItemVectorT<Face>& vector)
{
  for(FaceEnumerator iface(node.faces());iface();++iface)
    addChildFaces(*iface,vector) ;
}
inline void amrCells(Node const& node, ItemVectorT<Cell>& vector)
{
  List<Cell> cells_for_hanging_node;
  //for(FaceEnumerator iface=faces.enumerator();iface();++iface){
  for(FaceEnumerator iface(node.faces());iface();++iface){
    const Face& face = *iface;
    if(face.isSubDomainBoundary()) {
      const Cell cell0 = face.boundaryCell();
      bool add_hang=true;
      //if(cell0.isActive())
        for (List<Cell>::const_iterator iterator = cells_for_hanging_node.begin();
                                        iterator != cells_for_hanging_node.end(); ++iterator){
          if(*iterator == cell0)
            add_hang=false;
        }
        if(add_hang){
          cells_for_hanging_node.add(cell0);
          vector.addItem(cell0);
        }

    }else {
      const Cell cellback       = face.backCell();
      const Cell cellfront      = face.frontCell();
      bool add_hang_back=true;
      bool add_hang_front=true;
      for (List<Cell>::const_iterator iterator = cells_for_hanging_node.begin();
                                      iterator != cells_for_hanging_node.end(); ++iterator){
        if(*iterator == cellback)
          add_hang_back=false;
        if(*iterator == cellfront)
         add_hang_front=false;
      }
      //if(cellback.isActive())
        if(add_hang_back){
          cells_for_hanging_node.add(cellback);
          vector.addItem(cellback);
        }
     // if(cellfront.isActive())
        if(add_hang_front){
          cells_for_hanging_node.add(cellfront);
          vector.addItem(cellfront);
        }
     }
  }
}

template<typename DomainT>
Cell boundaryCell(DomainT const& domain, Face const& face)
{
  return domain.boundaryCell(face) ;
}

#if (ARCANE_VERSION < 30003)
template<typename DomainT>
ItemGroupT<DualNode> dualHybridSubGroup(const DomainT& domain,
                                        ItemGroupT<Cell> const& group,
                                        bool with_boundary_face)
{
   return domain.getHybridDualSubGroup(group,group.name(),with_boundary_face) ;
}

template<typename DomainT>
ItemGroupT<DualNode> dualHybridSubGroup(const DomainT& domain,
                                        ItemGroupT<Face> const& group,
                                        bool with_boundary_face)
{
   return domain.getHybridDualSubGroup(group,group.name(),with_boundary_face) ;
}
#endif
END_MESH_NAMESPACE
END_ARCGEOSIM_NAMESPACE
#endif /* AMRUTILS_H_ */
