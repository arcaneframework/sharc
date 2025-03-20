// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef FUNCTIONVARS_
#define FUNCTIONVARS_

#include "ArcGeoSim/Utils/ArcGeoSim.h"

#include "boost/tuple/tuple.hpp"

BEGIN_ARCGEOSIM_NAMESPACE
BEGIN_NUMERICS_NAMESPACE
BEGIN_EXPRESSIONS_NAMESPACE



template<typename ValueType,Integer N>
struct Vars
{
} ;

template<typename ValueType>
struct Vars<ValueType,1>
{
  typedef ValueType var_type;
} ;

template<typename ValueType>
struct Vars<ValueType,2>
{
  typedef typename boost::tuple<ValueType&,ValueType&> var_type ;
} ;

template<typename ValueType>
struct Vars<ValueType,3>
{
  typedef typename boost::tuple<ValueType&,ValueType&,ValueType&> var_type ;
} ;


template<typename ValueType>
class DVar
{
public :
  DVar()
  : m_y(ValueType())
  , m_dy(ValueType())
  {}

  DVar(Real y, Real dy)
  : m_y(y)
  , m_dy(dy)
  {}
  ValueType m_y ;
  ValueType m_dy ;
} ;

template<typename ValueType, Integer N>
struct DVarN
{
ValueType y ;
ValueType dy[N] ;
} ;


END_EXPRESSIONS_NAMESPACE
END_NUMERICS_NAMESPACE
END_ARCGEOSIM_NAMESPACE
#endif /*FUNCTIONVARS_*/
