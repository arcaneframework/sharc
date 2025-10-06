// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_PHYSICS_LAW_VARIABLERESIZER_H
#define ARCGEOSIM_PHYSICS_LAW_VARIABLERESIZER_H

/*
 * \ingroup Law
 * \brief Outil 
 *
 * \code 
 * \endcode
 *
 */

#include "ArcGeoSim/Physics/Law2/Property.h"
#include "ArcGeoSim/Physics/Law2/Enumerator.h"

#include <map>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*
 * \ingroup Law
 * \brief Accesseur g�n�rique
 *
 */

template<typename P> struct VariableResizer;

template<typename T>
struct VariableResizer< PropertyT<eScalar,T> >
{
  template<typename V>
  static void resize(const Property& p, V& v) {}

  template<typename V>
  static void resizeDerivatives(const Property& p, Integer size, V& v, const GroupEnumerator& enumerator)
  {
    v.resize(size);
    ENUMERATE_ITEM(iitem, enumerator.m_group)  v[iitem].fill(0);
  }

  template<typename V>
  static void resizeDerivativesIfSmaller(Integer size, V& v)
  {
	  if(v.arraySize()<size){
		  v.resize(size);
	  }
  }

  template<typename V>
  static void initializeDerivatives(V& v, Integer index, T value, const GroupEnumerator& enumerator)
  {
	  ENUMERATE_ITEM(iitem, enumerator.m_group)  v[iitem][index]=value;
  }
};

/*---------------------------------------------------------------------------*/

template<typename T>
struct VariableResizer< PropertyT<eVectorial,T> >
{
  template<typename V>
  static void resize(const Property& p, V& v) 
  { 
    v.resize(p.size()); 
  }
  
  template<typename V>
  static void resizeDerivatives(const Property& p, Integer size, V& v, const GroupEnumerator& enumerator)
  {
    v.resize(p.size(), size);
    ENUMERATE_ITEM(iitem, enumerator.m_group)
    {
      Array2View<T> view(v[iitem]);
      for(Integer i = 0; i < view.dim1Size(); ++i) view[i].fill(0);
    }
  }

  template<typename V>
  static void resizeDerivativesIfSmaller(Integer size, V& v)
  {
	  if(v.dim2Size()<size){
		  v.resize(v.dim1Size(), size);
	  }
  }

  template<typename V>
  static void initializeDerivatives(V& v, Integer index, T value, const GroupEnumerator& enumerator)
  {
	  ENUMERATE_ITEM(iitem, enumerator.m_group){
		  Array2View<T> view(v[iitem]);
		  // todo verifier bug vectoriel
		  view[index].fill(value);
	  }
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename P> struct ArrayResizer;

template<typename T>
struct ArrayResizer< PropertyT<eScalar,T> >
{
  template<typename V>
  static void resize(const Property& p, Integer size, V& v) 
  {
    v.resize(size); 
  }

  template<typename V>
  static void resizeDerivatives(const Property& p, Integer size, V& v, const RangeEnumerator& enumerator)
  {
    v.resize(v.dim1Size(), size);
    for(Integer i = enumerator.m_begin; i < enumerator.m_end; ++i)
      v[i].fill(0);
  }

  template<typename V>
  static void resizeDerivativesIfSmaller(Integer size, V& v)
  {
	  if(v.dim2Size()<size){
		  v.resize(v.dim1Size(), size);
	  }
  }

  template<typename V>
  static void initializeDerivatives(V& v, Integer index, T value, const RangeEnumerator& enumerator)
  {
	  for(Integer i = enumerator.m_begin; i < enumerator.m_end; ++i)
	    v[i][index]= value;
  }
};

/*---------------------------------------------------------------------------*/

template<typename T>
struct ArrayResizer< PropertyT<eVectorial,T> >
{
  template<typename V>
  static void resize(const Property& p, Integer size, V& v) 
  {
    v.resize(size, p.size()); 
  }
  
  template<typename V>
  static void resizeDerivatives(const Property& p, Integer size, V& v, const RangeEnumerator& enumerator)
  {
    v.resize(v.dim1Size(), p.size(), size);
    for(Integer i = enumerator.m_begin; i < enumerator.m_end; ++i)
    {
      Array2View<T> view(v[i]);
      for(Integer j = 0; j < view.dim1Size(); ++j) view[j].fill(0);
    }
  }

  template<typename V>
  static void resizeDerivativesIfSmaller(Integer size, V& v)
  {
	  if(v.dim3Size()<size){
		  v.resize(v.dim1Size(), v.dim2Size(), size);
	  }
  }

  template<typename V>
  static void initializeDerivatives(V& v, Integer index, T value, const RangeEnumerator& enumerator)
  {
	  for(Integer i = enumerator.m_begin; i < enumerator.m_end; ++i){
		  Array2View<T> view(v[i]);
		  view[index].fill(value);
	  }
  }
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

template<typename P> struct ScalarResizer;

template<typename T>
struct ScalarResizer< PropertyT<eScalar,T> >
{
  template<typename V>
  static void resize(const Property& p, V& v) {}

  template<typename V>
  static void resizeDerivatives(const Property& p, Integer size, V& v, const NoneEnumerator& enumerator)
  {
    v.resize(size);
    v.fill(0.);
  }

  template<typename V>
  static void resizeDerivativesIfSmaller(Integer size, V& v)
  {
    if(v.arraySize()<size){
      v.resize(size);
    }
  }

  template<typename V>
  static void initializeDerivatives(V& v, Integer index, T value, const NoneEnumerator& enumerator)
  {
    v[index]=value;
  }
};

/*---------------------------------------------------------------------------*/

template<typename T>
struct ScalarResizer< PropertyT<eVectorial,T> >
{
  template<typename V>
  static void resize(const Property& p, V& v)
  {
    v.resize(p.size());
  }

  template<typename V>
  static void resizeDerivatives(const Property& p, Integer size, V& v, const NoneEnumerator& enumerator)
  {
    v.resize(p.size(), size);
    for(Integer i = 0; i < v.dim1Size(); ++i) v[i].fill(0);
  }

  template<typename V>
  static void resizeDerivativesIfSmaller(Integer size, V& v)
  {
    if(v.dim2Size()<size){
      v.resize(v.dim1Size(), size);
    }
  }

  template<typename V>
  static void initializeDerivatives(V& v, Integer index, T value, const NoneEnumerator& enumerator)
  {
    // todo verifier bug vectoriel
    v[index].fill(value);
  }
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_LAW_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_PHYSICS_LAW_VARIABLERESIZER_H */
