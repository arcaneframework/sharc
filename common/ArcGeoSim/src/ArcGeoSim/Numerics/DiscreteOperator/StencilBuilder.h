// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef STENCILBUILDER_H
#define STENCILBUILDER_H

#include <algorithm>
#include <vector>

#include "ArcGeoSim/Numerics/DiscreteOperator/BinarySearch.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/CoefficientArray.h"
#include "ArcGeoSim/Numerics/DiscreteOperator/LinearSearch.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
class StencilSizesBuilderT
{
public:
  StencilSizesBuilderT(CoefficientArrayT<T> * a_coefficient_array)
    : m_coefficient_array(a_coefficient_array)
  {
    m_sizes.init(a_coefficient_array->faceGroup());
    m_sizes.fill(0);
  }

  void insert(const Face & a_face, const T & a_item, const Real & a_value = 0.);
  void finalize();
private:
  CoefficientArrayT<T> * m_coefficient_array;
  ItemGroupMapT<Face, Integer> m_sizes;
};

/*----------------------------------------------------------------------------*/

template<typename T>
void StencilSizesBuilderT<T>::insert(const Face & a_face, const T & a_item, const Real & a_value)
{
  m_sizes[a_face]++;
}

/*----------------------------------------------------------------------------*/

template<typename T>
void StencilSizesBuilderT<T>::finalize()
{
  m_coefficient_array->init(m_sizes);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template<typename T>
class OrderedStencilBuilderT
{
  typedef std::vector<Integer> TemporaryStencilType;  

public:
  typedef typename CoefficientArrayT<T>::StencilLocalIdType StencilLocalIdType;
  typedef BinarySearchT<StencilLocalIdType> SearchAlgorithm;
  
public:
  OrderedStencilBuilderT(CoefficientArrayT<T> * a_coefficient_array)
    : m_coefficient_array(a_coefficient_array),
      m_temporary(a_coefficient_array->faceGroup())
  {
    const FaceGroup & faces = a_coefficient_array->faceGroup();
    ENUMERATE_FACE(iface, faces) {
      const Face & F = *iface;      
      m_temporary[iface].reserve(a_coefficient_array->stencilSize(F));
    }
  }

  void insert(const Face & a_face, const T & a_item, const Real & a_value = 0.);
  void finalize();
private:
  CoefficientArrayT<T> * m_coefficient_array;
  ItemGroupMapT<Face, TemporaryStencilType> m_temporary;
};

/*----------------------------------------------------------------------------*/

template<typename T>
void OrderedStencilBuilderT<T>::insert(const Face & a_face, const T & a_item, const Real & a_value)
{
  TemporaryStencilType & stencil_face_temp = m_temporary[a_face];
  TemporaryStencilType::iterator i = 
    std::lower_bound(stencil_face_temp.begin(), stencil_face_temp.end(), a_item.localId());
  if(i==stencil_face_temp.end() || a_value<*i)
    stencil_face_temp.insert(i, a_item.localId());
}

/*----------------------------------------------------------------------------*/

template<typename T>
void OrderedStencilBuilderT<T>::finalize()
{
  const FaceGroup & faces = m_coefficient_array->faceGroup();
  ENUMERATE_FACE(iface, faces) {
    const Face & F = *iface;
    StencilLocalIdType stencil_face = m_coefficient_array->stencilLocalId(F);
    const TemporaryStencilType & stencil_face_temp = m_temporary[iface];
    copy(stencil_face_temp.begin(), stencil_face_temp.end(), stencil_face.begin());
  }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

class CooresCellStencilBuilder
{
  typedef std::vector<Integer> TemporaryStencilType;  

public:
  typedef CoefficientArrayT<Cell>::StencilLocalIdType StencilLocalIdType;
  typedef LinearSearchT<StencilLocalIdType> SearchAlgorithm;

public:
  CooresCellStencilBuilder(CoefficientArrayT<Cell> * a_coefficient_array)
    : m_coefficient_array(a_coefficient_array),
      m_temporary(a_coefficient_array->faceGroup())
  {
    const FaceGroup & faces = a_coefficient_array->faceGroup();
    ENUMERATE_FACE(iface, faces) {
      const Face & F = *iface;
      TemporaryStencilType & stencil_face_temp = m_temporary[iface];
      if(F.isSubDomainBoundary())
        stencil_face_temp.reserve(a_coefficient_array->stencilSize(F)-1);
      else
        stencil_face_temp.reserve(a_coefficient_array->stencilSize(F)-2);
    }
  }

  void insert(const Face & a_face, const Cell & a_item, const Real & a_value = 0.);
  void finalize();
private:
  CoefficientArrayT<Cell> * m_coefficient_array;
  ItemGroupMapT<Face, TemporaryStencilType> m_temporary;
};

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#endif
