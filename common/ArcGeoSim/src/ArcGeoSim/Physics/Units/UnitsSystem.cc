// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/PropertyHolder.h"
#include "IUnitsSystem.h"
#include "UnitsSystem.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
UnitsSystemService::
init()
{
//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "TODO: Ne g���������������������������re pas les coefficients additifs dans les conversions"
//#endif
//#endif
  if(options()->multFactor.size()>0)
    {
      for(Integer  ifactor=0;ifactor<options()->multFactor.size();ifactor++)
        {
          m_mult_factors.setProperty(options()->multFactor[ifactor]->unit(),
                                     options()->multFactor[ifactor]->value()) ;
          if(options()->multFactor[ifactor]->shortName.size()>0)
            m_short_name.setProperty(options()->multFactor[ifactor]->unit(),
                                     options()->multFactor[ifactor]->shortName[0]) ;
          if(options()->multFactor[ifactor]->longName.size()>0)
            m_long_name.setProperty(options()->multFactor[ifactor]->unit(),
                                    options()->multFactor[ifactor]->longName[0]) ;
          
        }
      for(Integer  ifactor=0;ifactor<options()->constFactor.size();ifactor++)
        {
          m_const_factors.setProperty(options()->constFactor[ifactor]->unit(),
                                     options()->constFactor[ifactor]->value()) ;
          if(options()->constFactor[ifactor]->shortName.size()>0)
            m_short_name.setProperty(options()->constFactor[ifactor]->unit(),
                                     options()->constFactor[ifactor]->shortName[0]) ;
          if(options()->constFactor[ifactor]->longName.size()>0)
            m_long_name.setProperty(options()->constFactor[ifactor]->unit(),
                                    options()->constFactor[ifactor]->longName[0]) ;
          
        }
    }
}

void
UnitsSystemService::
convert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  Real mul_factor = getMultFactor(unit) ;
  Real add_factor = getConstFactor(unit) ;
  for(Integer i=0;i<buf.size();i++) 
  {
      ptr[i] *= mul_factor ;
      ptr[i] += add_factor ;
  }
}

void
UnitsSystemService::
unconvert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  Real mul_factor = getMultFactor(unit) ;
  Real add_factor = getConstFactor(unit) ;
  for(Integer i=0;i<buf.size();i++)
  {
      ptr[i] -= add_factor ;
      ptr[i] /= mul_factor ;
  }
}

bool
UnitsSystemService::
hasMultFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return getMultFactor(unit) != system->getMultFactor(unit) ;
}

Real
UnitsSystemService::
getMultFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return getMultFactor(unit)/system->getMultFactor(unit); 
}

bool
UnitsSystemService::
hasConstFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return getConstFactor(unit) != system->getConstFactor(unit);
}

Real
UnitsSystemService::
getConstFactorTo(IUnitsSystem* system, UnitType unit) 
{ 
  return (getConstFactor(unit)-system->getConstFactor(unit))/system->getMultFactor(unit); 
}

Real 
UnitsSystemService::
convertTo(IUnitsSystem* system, IUnitsSystem::UnitType unit,Real val)
{ 
  return getMultFactorTo(system,unit)*val+getConstFactorTo(system,unit) ;
}

void 
UnitsSystemService::
convertTo(IUnitsSystem* system,IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactorTo(system,unit) ;
  const Real const_factor = getConstFactorTo(system,unit) ;
  for(Integer i=0;i<buf.size();i++)
    ptr[i] = const_factor+factor*ptr[i] ;
}

String 
UnitsSystemService::
getShortName(IUnitsSystem::UnitType unit)
{
  return m_short_name.getProperty(unit,IUnitsSystem::getSIUnitShortName(unit)) ;
}

String 
UnitsSystemService::
getLongName(IUnitsSystem::UnitType unit)
{
  return m_long_name.getProperty(unit,IUnitsSystem::getSIUnitLongName(unit)) ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_UNITSSYSTEM(UnitsSystem,UnitsSystemService);
