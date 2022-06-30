// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef FUNCTIONSUTILS_H_
#define FUNCTIONSUTILS_H_

#include "ArcGeoSim/Utils/Utils.h"

BEGIN_ARCGEOSIM_NAMESPACE

inline
void
normalize(ArrayView<Real> array, Integer size)
{
  Real total = 0 ;
  auto ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    total += *ptr ;
    ++ptr ;
  }
  ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    *ptr /= total ;
    ++ptr ;
  }
}
template<typename RealBaseArray>
inline
void
normalize(RealBaseArray& array, Integer size)
{
  Real total = 0 ;
  auto ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    total += *ptr ;
    ++ptr ;
  }
  ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    *ptr /= total ;
    ++ptr ;
  }
}

inline
void
normalize(ArrayView<Real> array, Integer size, bool check)
{
  Real total = 0 ;
  auto ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    total += *ptr ;
    ++ptr ;
  }
  if((check)&&(total==0)) return ;
  ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    *ptr /= total ;
    ++ptr ;
  }
}

template<typename RealBaseArray>
inline
void
normalize(RealBaseArray& array, Integer size, bool check)
{
  Real total = 0 ;
  auto ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    total += *ptr ;
    ++ptr ;
  }
  if((check)&&(total==0)) return ;
  ptr = array.begin() ;
  for(Integer i=0;i<size;i++)
  {
    *ptr /= total ;
    ++ptr ;
  }
}
inline
Real
hat(Real y_min, Real y_max,Real y)
{
  return Arcane::math::max(y_min,Arcane::math::min(y,y_max)) ;
}

template<class T,class I>
T max(T a, T b,I idb,I* id)
{
  if(a<b)
  {
    *id = idb ;
    return b ;
  }
  else
    return a ;
}

END_ARCGEOSIM_NAMESPACE

#endif /*FUNCTIONSUTILS_H_*/
