// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_UTILS_ARRAYUTILS_H
#define ARCGEOSIM_UTILS_ARRAYUTILS_H

#ifdef WIN32
#include <ciso646>
#endif

#include <arcane/ArcaneVersion.h>
#if (ARCANE_VERSION >= 12201)
#else /* ARCANE_VERSION */
#include <arcane/utils/CArrayAll.h>
#endif /* ARCANE_VERSION */
#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/utils/Array.h>
#include <arcane/utils/StringBuilder.h>
#include <arcane/utils/String.h>

#include <arcane/utils/Real2.h>
#include <arcane/utils/Real2x2.h>
#include <arcane/utils/Real3.h>
#include <arcane/utils/Real3x3.h>

#include "ArcGeoSim/Utils/ArcGeoSim.h"

using Arcane::Byte;
using Arcane::Integer;
using Arcane::Real;
using Arcane::String;
using Arcane::Array;
using Arcane::ArrayView;
using Arcane::ConstArrayView;
using Arcane::StringBuilder;
using Arcane::ItemGroup;
using Arcane::FaceGroup;
using Arcane::NodeGroup;
using Arcane::CellGroup;
using Arcane::ItemGroupCollection;
using Arcane::ItemVector;

BEGIN_ARCGEOSIM_NAMESPACE

#ifdef USE_ARCANE_V3
#define DATAPTR(T) (T).data()
#else
#define DATAPTR(T) (T).begin()
#endif
/*---------------------------------------------------------------------------*/

//! Recherche d'un �l�ment dans un tableau
namespace ArrayScan
{
//! Recherche lin�aire de la valeur \a x dans un tableau non-ordonn�e \a v avec contr�le de validit�
/*! Ce type de recherche est performante sur des tableaux de petites tailles (effet cache)
 *  Si plusieurs instance de la valeur \a x existe, la premi�re sera trouv�e.
 */
template<typename T> inline Integer exhaustiveScan(const T & x, const ConstArrayView<T> & v);

//! Recherche lin�aire de la valeur \a x dans un tableau ordonn�e \a v avec contr�le de validit�
/*! Ce type de recherche est performante sur des tableaux de petites tailles (effet cache)
 *  Si plusieurs instance de la valeur \a x existe, la premi�re sera trouv�e.
 */
template<typename T> inline Integer linearScan(const T & x, const ConstArrayView<T> & v);
//! Recherche dichotomique de la valeur \a x dans un tableau ordonn�e \a v avec contr�le de validit�
/*! Cette recherche dichotomique est hybrid�e avec une recherche lin�aire par de meilleures performances
 *  sur toutes tailles de tableau
 *  Si plusieurs instance de la valeur \a x existe, une quelconque occurence sera trouv�e.
 */
template<typename T> inline Integer dichotomicScan(const T & x, const ConstArrayView<T> & v);

//! Recherche lin�aire de la position d'insertion de la valeur \a x dans un tableau ordonn�e \a v avec contr�le de validit�
/*! Ce type de recherche est performante sur des tableaux de petites tailles (effet cache)
 *  Si plusieurs instance de la valeur \a x existe, la premi�re sera trouv�e.
 */
template<typename T> inline Integer linearPositionScan(const T & x, const ConstArrayView<T> & v);
//! Recherche dichotomique de la  position d'insertion de la valeur \a x dans un tableau ordonn�e \a v avec contr�le de validit�
/*! Cette recherche dichotomique est hybrid�e avec une recherche lin�aire par de meilleures performances
 *  sur toutes tailles de tableau
 *  Si plusieurs instance de la valeur \a x existe, une quelconque occurence sera trouv�e.
 */
template<typename T> inline Integer dichotomicPositionScan(const T & x, const ConstArrayView<T> & v);

//! Recherche lin�aire de la borne inf�rieur de l'intervalle contenant la valeur \a x dans un tableau ordonn�e \a v sans contr�le de validit�
/*! L'absence de contr�le de validit� ne contr�le pas que le tableau est non vide et que
 *  l'�l�ment recherch� est dans l'intervalle d�fini par les extr�mit�s du tableau
 */
template<typename T> inline Integer linearIntervalScan(const T & x, const Integer n, const T * vptr);

//! Recherche dichotomique de la borne inf�rieur de l'intervalle contenant la valeur \a x dans un tableau ordonn�e \a v avec contr�le de validit�
/*! Cette recherche dichotomique est hybrid� avec une recherche lin�aire par de meilleures performances
 *  sur toutes tailles de tableau.
 *  L'absence de contr�le de validit� ne contr�le pas que le tableau est non vide et que
 *  l'�l�ment recherch� est dans l'intervalle d�fini par les extr�mit�s du tableau
 */
template<typename T> inline Integer dichotomicIntervalScan(const T & x, const Integer n, const T * vptr);
}

/*---------------------------------------------------------------------------*/

//! Conversion de tableau
namespace ArrayConversion
{

//! Write \a array in a String: each value is separated by a " "
  template<typename T> inline String arrayToStringT(const Array<T>& array);
}

/*---------------------------------------------------------------------------*/

namespace ArrayScan
{

template<typename T>
Integer
exhaustiveScan(const T & x, const ConstArrayView<T> & v)
{
  const Integer n = v.size();
  for(Integer i=0;i<n;++i)
    if (v[i] == x)
        return i;
  return -1;
}

template<typename T>
Integer
linearScan(const T & x, const ConstArrayView<T> & v)
{
  const Integer n = v.size();
  Integer index = 0;
  while(index < n and v[index] < x)
    {
      ++index;
    }
  if (index == n or v[index] != x)
    return -1;
  else
    return index;
}

template<typename T>
inline Integer
dichotomicScan(const T & x, const ConstArrayView<T> & v)
{
  const Integer n = v.size();

  Integer ileft = 0;
  Integer iright = n; // d�finit un intervale ouvert � droite
  static const Integer n_linear = 20; // threshold to switch from dichotomic to linear scan \todo mettre cache line size

  // Start dichotomy
  if (n > n_linear) {
    if (x < v[0] or x > v[n-1]) return -1;
    do {
      const Integer imid = (iright+ileft)/2;
      const T & vmid = v[imid];
      if (x < vmid)
        {
          iright = imid;
        }
      else if (x >= vmid)
        {
          ileft = imid;
        }
    } while (iright - ileft > n_linear);
  }

  // Switch to linear search
  // (exhaustiveScan en alternative; gain mitig�)
  while(ileft < iright and v[ileft] < x)
    {
      ++ileft;
    }
  if (ileft >= iright or v[ileft] != x)
    return -1;
  else
    return ileft;
}

/*---------------------------------------------------------------------------*/

template<typename T>
Integer
linearPositionScan(const T & x, const ConstArrayView<T> & v)
{
  const Integer n = v.size();
  Integer index = 0;
  while(index < n and v[index] < x)
    {
      ++index;
    }
  return index;
}

template<typename T>
inline Integer
dichotomicPositionScan(const T & x, const ConstArrayView<T> & v)
{
  const Integer n = v.size();

  Integer ileft = 0;
  Integer iright = n; // d�finit un intervale ouvert � droite
  static const Integer n_linear = 20; // threshold to switch from dichotomic to linear scan \todo mettre cache line size

  // Start dichotomy
  if (n > n_linear) {
    if (x < v[0]) return 0;
    if (x > v[n-1]) return n;
    do {
      const Integer imid = (iright+ileft)/2;
      const T & vmid = v[imid];
      if (x < vmid)
        {
          iright = imid;
        }
      else if (x >= vmid)
        {
          ileft = imid;
        }
    } while (iright - ileft > n_linear);
  }

  // Switch to linear search
  // (exhaustiveScan en alternative; gain mitig�)
  while(ileft < iright and v[ileft] < x)
    {
      ++ileft;
    }
  return ileft;
}

/*---------------------------------------------------------------------------*/

template<typename T>
Integer
linearIntervalScan(const T & x, const Integer n, const T * vptr)
{
  // Prepare
  ARCANE_ASSERT((n>1),("Empty interval array not allowed"));
  ARCANE_ASSERT((x>=vptr[0] and x<vptr[n-1]),("Out of range value"));
  Integer index = 0;

  // Search
  while (vptr[index + 1] <= x)
    ++index;
  return index;
}

template<typename T>
Integer
dichotomicIntervalScan(const T & x, const Integer n, const T * vptr)
{
  //! Prepare
  ARCANE_ASSERT((n>1),("Empty interval array not allowed"));
  ARCANE_ASSERT((x>=vptr[0] and x <vptr[n-1]),("Out of range value"));

  Integer ileft = 0;
  Integer iright = n; // d�finit un intervalle ouvert � droite
  static const Integer n_linear = 20; // threshold to switch from dichotomic to linear scan \todo mettre cache line size

  // Start dichotomy
  while (iright - ileft > n_linear)
    {
      const Integer imid = (iright + ileft) / 2;
      const T & vmid = vptr[imid];
      if (x < vmid)
        {
          iright = imid;
        }
      else
        {
          ileft = imid;
        }
    }

  // Switch to linear search
  while (vptr[ileft + 1] <= x)
    ++ileft;
  return ileft;
}

} // end of namespace ArrayScan

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArrayConversion
{

template<typename T>
String
arrayToStringT(const Array<T>& array)
{
  StringBuilder sb;
  typename Array<T>::const_iterator ite = array.begin();
  for (; ite != array.end(); ++ite)
    {
    sb += " ";
    sb += *ite;
  }
  return sb.toString();
}

/*---------------------------------------------------------------------------*/

// Specialization for peculiar types
template<>
inline
String
arrayToStringT<Real2>(const Array<Real2>& array)
{
  StringBuilder sb;
  SharedArray<Real2>::const_iterator ite = array.begin();
  for (; ite != array.end(); ++ite)
    {
    sb += " ";
    sb += ite->x;
    sb += " ";
    sb += ite->y;
  }
  return sb.toString();
}

/*---------------------------------------------------------------------------*/

template<>
inline
String
arrayToStringT<Real3>(const Array<Real3>& array)
{
  StringBuilder sb;
  SharedArray<Real3>::const_iterator ite = array.begin();
  for (; ite != array.end(); ++ite)
    {
      sb += " ";
      sb += ite->x;
      sb += " ";
      sb += ite->y;
      sb += " ";
      sb += ite->z;
  }
  return sb.toString();
}

/*---------------------------------------------------------------------------*/

template<>
inline
String
arrayToStringT<Real2x2>(const Array<Real2x2>& array)
{
  StringBuilder sb;
  SharedArray<Real2x2>::const_iterator ite = array.begin();
  for (; ite != array.end(); ++ite)
    {
      sb += " ";
      sb += ite->x.x;
      sb += " ";
      sb += ite->x.y;
      sb += " ";
      sb += ite->y.x;
      sb += " ";
      sb += ite->y.y;
  }
  return sb.toString();
}

/*---------------------------------------------------------------------------*/

template<>
inline
String
arrayToStringT<Real3x3>(const Array<Real3x3>& array)
{
  StringBuilder sb;
  SharedArray<Real3x3>::const_iterator ite = array.begin();
  for (; ite != array.end(); ++ite)
    {
      sb += " ";
      sb += ite->x.x;
      sb += " ";
      sb += ite->x.y;
      sb += " ";
      sb += ite->x.z;
      sb += " ";
      sb += ite->y.x;
      sb += " ";
      sb += ite->y.y;
      sb += " ";
      sb += ite->y.z;
      sb += " ";
      sb += ite->z.x;
      sb += " ";
      sb += ite->z.y;
      sb += " ";
      sb += ite->z.z;
  }
  return sb.toString();
}

/*---------------------------------------------------------------------------*/

} // end of namespace ArrayConversion

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline
void insert(Array<Integer>& list,
            Array<Real>& value,
            Integer entry,
            Real eps=0.)
{
  if(entry==0)
  {
      list[0] = 0 ;
      return ;
  }
  Integer size = entry ;
  Integer i = size ;
  Real z = value[entry] ;
  for(Integer k=0;k<size;k++)
  {
    if(z-value[list[k]]>=eps)
    { 
      i=k ;
      break ;
    }
  }
  Integer last = entry ;
  for(Integer j=i;j<size;j++)
  {
      Integer tmp = list[j] ;
      list[j] = last ;
      last = tmp ; 
  }
  list[size] = last ;
}

inline
Real average(ArrayView<Real> x,
             ArrayView<Real> coef,
             Integer n)
{
  Real xx = 0. ;
  for(Integer i=0;i<n;i++)
    xx += coef[i]*x[i] ;
  return xx ;
}

/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

#endif /*ARCGEOSIM_UTILS_ARRAYUTILS_H*/
