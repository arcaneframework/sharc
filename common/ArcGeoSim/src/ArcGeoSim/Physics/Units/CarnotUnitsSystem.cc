// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/PropertyHolder.h"
#include "IUnitsSystem.h"
#include "CarnotUnitsSystem.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
CarnotUnitsSystem::
init()
{
    m_mult_factors.setProperty(IUnitsSystemProperty::Salinity,1.e3) ;
}

bool
CarnotUnitsSystem::
hasMultFactor(IUnitsSystem::UnitType unit)
{
  return (m_mult_factors.hasProperty(unit)) ;
}

Real
CarnotUnitsSystem::
getMultFactor(IUnitsSystem::UnitType unit)
{
  return m_mult_factors.getProperty(unit,1.) ;
}

bool
CarnotUnitsSystem::
hasConstFactor(IUnitsSystem::UnitType unit)
{
  return (m_const_factors.hasProperty(unit)) ;
}

Real
CarnotUnitsSystem::
getConstFactor(IUnitsSystem::UnitType unit)
{
  return m_const_factors.getProperty(unit,0.) ;
}

bool
CarnotUnitsSystem::
hasMultFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return getMultFactor(unit) != system->getMultFactor(unit) ;
}

Real
CarnotUnitsSystem::
getMultFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return getMultFactor(unit)/system->getMultFactor(unit); 
}

bool
CarnotUnitsSystem::
hasConstFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return getConstFactor(unit) != system->getConstFactor(unit);
}

Real
CarnotUnitsSystem::
getConstFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return (getConstFactor(unit)-system->getConstFactor(unit))/system->getMultFactor(unit); 
}

Real CarnotUnitsSystem::convert(IUnitsSystem::UnitType unit,Real val)
{
  return m_const_factors.getProperty(unit,0.)+val*getMultFactor(unit) ;
}

Real CarnotUnitsSystem::unconvert(IUnitsSystem::UnitType unit,Real val)
{
  return (val-m_const_factors.getProperty(unit,0.))/getMultFactor(unit) ;
}

void CarnotUnitsSystem::convert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactor(unit) ;
  const Real const_factor = getConstFactor(unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

void CarnotUnitsSystem::unconvert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactor(unit) ;
  const Real const_factor = getConstFactor(unit) ;
  for(Integer i=0;i<buf.size();i++)
  {
      ptr[i] -= const_factor ;
      ptr[i] /= factor ;
  }
}

Real 
CarnotUnitsSystem::
convertTo(IUnitsSystem* system, IUnitsSystem::UnitType unit,Real val)
{ 
  return getMultFactorTo(system,unit)*val + getConstFactorTo(system,unit);
}

void 
CarnotUnitsSystem::
convertTo(IUnitsSystem* system,IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactorTo(system,unit) ;
  const Real const_factor = getConstFactorTo(system,unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

