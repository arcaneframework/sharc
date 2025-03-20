// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef NORME_H_
#define NORME_H_

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/ItemGroupMap.h"

#include <arcane/VariableTypes.h>

namespace Arcane {
  template <typename ItemT, typename DataT> class MeshVariableScalarRefT ;
}
BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_NUMERICS_NAMESPACE

typedef enum {
  L0,
  L1,
  L2
} eNormeType ;

template<typename DataType,
         typename ItemType>
Real normeL0(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    norme = Arcane::math::max(norme,var[item]) ;
  }
  return norme ;
}

template<typename DataType,
         typename ItemType>
Real normeL0(ItemGroupMapT<ItemType,DataType>& var,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    norme = Arcane::math::max(norme,var[*item]) ;
  }
  return norme ;
}

template<typename DataType,
         typename ItemType>
Real normeL0(Arcane::MeshVariableArrayRefT<ItemType,DataType>& var,
             Integer size,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    ArrayView<Real> v = var[item] ;
    for(Integer i=0;i<size;++i)
    {
      norme = Arcane::math::max(norme,v[i]) ;
    }
  }
  return norme ;
}

template<typename DataType,
         typename ItemType>
Real normeL0(ItemGroupMapArrayT<ItemType,DataType>& var,
             Integer size,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    ArrayView<Real> v = var[*item] ;
    for(Integer i=0;i<size;++i)
    {
      norme = Arcane::math::max(norme,v[i]) ;
    }
  }
  return norme ;
}

template<typename DataType,
         typename ItemType>
Real normeL1(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    norme += Arcane::math::abs(var[item]) ;
  }
  return norme ;
}
template<typename DataType,
         typename ItemType>
Real normeL1(ItemGroupMapT<ItemType,DataType>& var,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    norme += Arcane::math::abs(var[*item]) ;
  }
  return norme ;
}
template<typename DataType,
         typename ItemType>
Real normeL1(Arcane::MeshVariableArrayRefT<ItemType,DataType>& var,
             Integer size,
            const Arcane::ItemGroupT<ItemType>& group)
{
  DataType norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    ArrayView<Real> v = var[item] ;
    for(Integer i=0;i<size;++i)
    {
      norme += Arcane::math::abs(v[i]) ;
    }
  }
  return norme ;
}
template<typename DataType,
         typename ItemType>
Real normeL1(ItemGroupMapArrayT<ItemType,DataType>& var,
             Integer size,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    ArrayView<Real> v = var[*item] ;
    for(Integer i=0;i<size;++i)
    {
      norme += Arcane::math::abs(v[i]) ;
    }
  }
  return norme ;
}


template<typename DataType,
         typename ItemType>
Real normeL2(Arcane::MeshVariableScalarRefT<ItemType,DataType>& var,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    Real val = var[item] ;
    norme += val*val ;
  }
  return Arcane::math::sqrt(norme) ;
}

template<typename DataType,
         typename ItemType>
Real normeL2(ItemGroupMapT<ItemType,DataType>& var,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    Real val = var[*item] ;
    norme += val*val ;
  }
  return Arcane::math::sqrt(norme) ;
}

template<typename DataType,
         typename ItemType>
Real normeL2(Arcane::MeshVariableArrayRefT<ItemType,DataType>& var,
             Integer size,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    ArrayView<Real> v = var[item] ;
    for(Integer i=0;i<size;++i)
    {
      Real val = v[i] ;
      norme += val*val ;
    }
  }
  return Arcane::math::sqrt(norme) ;
}

template<typename DataType,
         typename ItemType>
Real normeL2(ItemGroupMapArrayT<ItemType,DataType>& var,
             Integer size,
            const Arcane::ItemGroupT<ItemType>& group)
{
  Real norme = 0 ;
  for( Arcane::ItemEnumeratorT<ItemType> item((group).enumerator());
       item.hasNext(); ++item )
  {
    ArrayView<Real> v = var[*item] ;
    for(Integer i=0;i<size;++i)
    {
      Real val = v[i] ;
      norme += val*val ;
    }
  }
  return Arcane::math::sqrt(norme) ;
}

template<eNormeType norme_type> class Norme ;

template<>
class
Norme<L0>
{
public :
  Norme(){}
  virtual ~Norme(){}
  template<typename ItemType>
  Real eval(Arcane::MeshVariableScalarRefT<ItemType,Real>& var,
            Arcane::ItemGroupT<ItemType>& group)
  {
    return normeL0<Real,ItemType>(var,group) ;
  }
  template<typename ItemType>
  Real eval(Arcane::MeshVariableArrayRefT<ItemType,Real>& var,
            Integer size,
            Arcane::ItemGroupT<ItemType> group)
  {
    return normeL0<Real,ItemType>(var,size,group) ;
  }
  template<typename ItemType>
  Real eval(ItemGroupMapT<ItemType,Real>& var,
            Arcane::ItemGroupT<ItemType> group) {
    return normeL0<Real,ItemType>(var,group) ;
  }
  template<typename ItemType>
  Real eval(ItemGroupMapArrayT<ItemType,Real>& var,
            Integer size,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL0<Real,ItemType>(var,size,group) ;
  }
} ;

template<>
class
Norme<L1>
{
public :
  Norme(){}
  virtual ~Norme(){}

  template<typename ItemType>
  Real eval(Arcane::MeshVariableScalarRefT<ItemType,Real>& var,
            Arcane::ItemGroupT<ItemType>& group)
  {
    return normeL1<Real,ItemType>(var,group) ;
  }
  template<typename ItemType>
  Real eval(Arcane::MeshVariableArrayRefT<ItemType,Real>& var,
            Integer size,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL1<Real,ItemType>(var,size,group) ;
  }
  template<typename ItemType>
  Real eval(ItemGroupMapT<ItemType,Real>& var,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL1<Real,ItemType>(var,group) ;
  }
  template<typename ItemType>
  Real eval(ItemGroupMapArrayT<ItemType,Real>& var,
            Integer size,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL1<Real,ItemType>(var,size,group) ;
  }
} ;

template<>
class
Norme<L2>
{
public :
  Norme(){}
  virtual ~Norme(){}

  template<typename ItemType>
  Real eval(Arcane::MeshVariableScalarRefT<ItemType,Real>& var,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL2<Real,ItemType>(var,group) ;
  }
  template<typename ItemType>
  Real eval(Arcane::MeshVariableArrayRefT<ItemType,Real>& var,
            Integer size,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL2<Real,ItemType>(var,size,group) ;
  }

  template<typename ItemType>
  Real eval(ItemGroupMapT<ItemType,Real>& var,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL2<Real,ItemType>(var,group) ;
  }
  template<typename ItemType>
  Real eval(ItemGroupMapArrayT<ItemType,Real>& var,
            Integer size,
            Arcane::ItemGroupT<ItemType>& group) {
    return normeL2<Real,ItemType>(var,size,group) ;
  }
} ;

END_NUMERICS_NAMESPACE
END_ARCGEOSIM_NAMESPACE

#endif /*NORME_H_*/
