// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_NUMERICS_UTILS_INTERPOLATIONUTILS_H
#define ARCGEOSIM_NUMERICS_UTILS_INTERPOLATIONUTILS_H
/* Author : dechaiss at Fri Apr 15 09:49:29 2011
 * Generated by createNew
 */

#include <arcane/utils/ArcaneGlobal.h>
#include <arcane/datatype/DataTypeTraits.h>
#include <arcane/ArcaneException.h>
#include "ArcGeoSim/Utils/ArcGeoSim.h"

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/

// Tools to realize Interpolation
//! Outils d'interpolation
// declaration
namespace InterpolationUtils
{

/*! Traits pour classer en deux categories les types bases sur des reels (Real,Real2,Real3...) et les autres
 * Les types bases sur des reels ont le typedef InterpolationTypeTraits::isReal valant TrueType et les autres
 * FalseType
 * Ensuite on utilisera des signatures de fonctions differentes pour aiguiller entre ces deux familles.
 */
template<typename T>
class InterpolationTypeTraits
{
public:
  typedef Arcane::FalseType isReal;
};


//! Generation automatique des specialisations pour les types Reels
#define REAL_BASED_TYPE_INTERPOLATION_TRAITS(type)\
template<> \
class InterpolationTypeTraits<type> \
{\
public:\
  typedef Arcane::TrueType isReal;\
};

REAL_BASED_TYPE_INTERPOLATION_TRAITS(Real)
REAL_BASED_TYPE_INTERPOLATION_TRAITS(Real2)
REAL_BASED_TYPE_INTERPOLATION_TRAITS(Real3)
REAL_BASED_TYPE_INTERPOLATION_TRAITS(Real3x3)



/*! Retourne l'interpole f(x) donne par :
 * f(x) = \a y_a + (\a x - \a x_a) * (\a y_b - \a y_a)/(\a x_b - \a x_a)
 * on suppose que les abscisses sont des reelles et que les ordonnees sont des types
 * bases sur des reels.
 */
template<typename T> inline T linearInterpolation(const Real& x,
                                           const Real& x_a,
                                           const Real& x_b,
                                           const T& y_a,
                                           const T& y_b);

//! Implementation pour les donnees basees sur des Reels (Real, Real2, Real3...)
template<typename T> inline T _linearInterpolation(const Real& x,
                                            const Real& x_a,
                                            const Real& x_b,
                                            const T& y_a,
                                            const T& y_b,
                                            const Arcane::TrueType& type);

//! Implementation pour les autres types : ne doit pas etre appelee, l'interpolation n'ayant pas de sens dans ce cas
template<typename T> inline T _linearInterpolation(const Real& x,
                                            const Real& x_a,
                                            const Real& x_b,
                                            const T& y_a,
                                            const T& y_b,
                                            const Arcane::FalseType& type);

}

// definition
namespace InterpolationUtils
{

template<typename T>
T
linearInterpolation(const Real& x,
                    const Real& x_a,
                    const Real& x_b,
                    const T& y_a,
                    const T& y_b)
{
  return InterpolationUtils::_linearInterpolation<T> (x,x_a,x_b,y_a,y_b, typename InterpolationTypeTraits<T>::isReal());
}

template<typename T>
T
_linearInterpolation(const Real& x,
                     const Real& x_a,
                     const Real& x_b,
                     const T& y_a,
                     const T& y_b,
                     const Arcane::TrueType& type)
{
  // Define interpolated value
  T y_interpolated;
  // Handle basic Real type and complex Real3, Real2 and Real3x3 types
  typedef typename DataTypeTraitsT<T>::BasicType BasicType;
  Integer nb_type = DataTypeTraitsT<T>::nbBasicType();
  // Reinterpret y_a, y_b and y_interpolated in basic type
  const BasicType* base_value_y_a = reinterpret_cast<const BasicType*>(&y_a);
  const BasicType* base_value_y_b = reinterpret_cast<const BasicType*>(&y_b);
  BasicType* base_value_y_interp = reinterpret_cast<BasicType*>(&y_interpolated);
  // Interpolate for each base value
  for (Integer i = 0; i< nb_type; ++i)
    {
      base_value_y_interp[i] = base_value_y_a[i]+ (x-x_a) * (base_value_y_b[i]-base_value_y_a[i]) /(x_b-x_a);
    }
  return y_interpolated;
}

template<typename T>
T
_linearInterpolation(const Real& x,
                     const Real& x_a,
                     const Real& x_b,
                     const T& y_a,
                     const T& y_b,
                     const Arcane::FalseType& type)
{
  throw InternalErrorException(A_FUNCINFO,"Cannot use interpolation with non Real based data (Real,Real2,Real3,Real3x3)");
  return T();
}

} // close InterpolationUtils namespace

END_ARCGEOSIM_NAMESPACE

#endif /* ARCGEOSIM_NUMERICS_UTILS_INTERPOLATIONUTILS_H */
