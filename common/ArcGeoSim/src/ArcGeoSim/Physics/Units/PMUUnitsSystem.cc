// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/PropertyHolder.h"
#include "IUnitsSystem.h"
#include "PMUUnitsSystem.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
PMUUnitsSystemService::
init()
{
	// jour -> seconde
    m_mult_factors.setProperty(IUnitsSystemProperty::Time,86400.) ;
    m_short_name.setProperty(IUnitsSystemProperty::Time,"D.") ;
    m_long_name.setProperty(IUnitsSystemProperty::Time,"day") ;

    // rad -> deg
    m_mult_factors.setProperty(IUnitsSystemProperty::Angle,0.01745329) ;
    m_short_name.setProperty(IUnitsSystemProperty::Angle,"deg") ;
    m_long_name.setProperty(IUnitsSystemProperty::Angle,"degree") ;

    // bar -> Pa
    m_mult_factors.setProperty(IUnitsSystemProperty::Pressure,1.E+05) ;
    m_short_name.setProperty(IUnitsSystemProperty::Pressure,"bar") ;
    m_long_name.setProperty(IUnitsSystemProperty::Pressure,"bar") ;

    m_mult_factors.setProperty(IUnitsSystemProperty::CapillaryPressure,1.E+05) ;
    m_short_name.setProperty(IUnitsSystemProperty::CapillaryPressure,"bar") ;
    m_long_name.setProperty(IUnitsSystemProperty::CapillaryPressure,"bar") ;

    // md -> m2
    m_mult_factors.setProperty(IUnitsSystemProperty::Permeability,9.869233E-16) ;
    m_short_name.setProperty(IUnitsSystemProperty::Permeability,"md") ;
    m_long_name.setProperty(IUnitsSystemProperty::Permeability,"milli darcy") ;

    // m2/d -> m2/s
    m_mult_factors.setProperty(IUnitsSystemProperty::Diffusion,1.157407E-5) ;
    m_short_name.setProperty(IUnitsSystemProperty::Diffusion,"m2/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::Diffusion,"square meter per day") ;

    // cm -> m
    m_mult_factors.setProperty(IUnitsSystemProperty::Radius,1.E-2) ;
    m_short_name.setProperty(IUnitsSystemProperty::Radius,"cm") ;
    m_long_name.setProperty(IUnitsSystemProperty::Radius,"cm") ;

    // g/cm3 -> kg/m3
    m_mult_factors.setProperty(IUnitsSystemProperty::MassDensity,1.E3) ;
    m_short_name.setProperty(IUnitsSystemProperty::MassDensity,"g.cm-3") ;
    m_long_name.setProperty(IUnitsSystemProperty::MassDensity,"gram per cm3") ;

    // cp -> P
    m_mult_factors.setProperty(IUnitsSystemProperty::Viscosity,1.E-3) ;
    m_short_name.setProperty(IUnitsSystemProperty::Viscosity,"cP") ;
    m_long_name.setProperty(IUnitsSystemProperty::Viscosity,"centi Poise") ;


    // g -> kg
    m_mult_factors.setProperty(IUnitsSystemProperty::MolarMass,1.E-3) ;
    m_short_name.setProperty(IUnitsSystemProperty::MolarMass,"g.mol-1") ;
    m_long_name.setProperty(IUnitsSystemProperty::MolarMass,"gram per mole") ;

    // cm3/mol -> m3/mol
    m_mult_factors.setProperty(IUnitsSystemProperty::MolarVol,1.E-6) ;
    m_short_name.setProperty(IUnitsSystemProperty::MolarVol,"cm3.mol-1") ;
    m_long_name.setProperty(IUnitsSystemProperty::MolarVol,"cubic centimeters per mole") ;

    // mol/d -> mol/s
    m_mult_factors.setProperty(IUnitsSystemProperty::Flow,1.157407e-5) ;
    m_short_name.setProperty(IUnitsSystemProperty::Flow,"mol/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::Flow,"mole per day") ;

    // kg/d -> kg/s
    m_mult_factors.setProperty(IUnitsSystemProperty::MassFlow,1.157407e-5) ;
    m_short_name.setProperty(IUnitsSystemProperty::MassFlow,"kg/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::MassFlow,"Kilogram per day") ;

    // m3/d -> m3/s
    m_mult_factors.setProperty(IUnitsSystemProperty::LiqVolFlow,1.157407e-5) ;
    m_short_name.setProperty(IUnitsSystemProperty::LiqVolFlow,"m3/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::LiqVolFlow,"cubic meters per day") ;

    // m3/d -> m3/s
    m_mult_factors.setProperty(IUnitsSystemProperty::GasVolFlow,1.157407e-5) ;
    m_short_name.setProperty(IUnitsSystemProperty::GasVolFlow,"m3/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::GasVolFlow,"cubic meters per day") ;

    // degre -> kelvin
    m_const_factors.setProperty(IUnitsSystemProperty::Temperature,273.15) ;
    m_short_name.setProperty(IUnitsSystemProperty::Temperature,"C") ;
    m_long_name.setProperty(IUnitsSystemProperty::Temperature,"Celcius") ;

    // degre/m -> kelvin/m
    m_short_name.setProperty(IUnitsSystemProperty::ThermalGradient,"C/m") ;
    m_long_name.setProperty(IUnitsSystemProperty::ThermalGradient,"Celcius per meter") ;

    // 1/bar -> 1/Pa
    m_mult_factors.setProperty(IUnitsSystemProperty::Compressibility,1.E-05) ;
    m_short_name.setProperty(IUnitsSystemProperty::Compressibility,"1/bar") ;
    m_long_name.setProperty(IUnitsSystemProperty::Compressibility,"1/bar") ;

    // J/(g.�C) -> J/(Kg.�K)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatMassCapacity,1.E+03) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatMassCapacity,"J/(g.C)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatMassCapacity,"joule per gram Celsius degree") ;

    // J/(g.�C2) -> J/(Kg.�K2)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatMassCapacityT,1.E+03) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatMassCapacityT,"J/(g.C2)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatMassCapacityT,"joule per gram square Celsius degree") ;

    // J/(cm3.�C) -> J/(m3.�K)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatVolCapacity,1.E+06) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatVolCapacity,"J/(cm3.C)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatVolCapacity,"joule per cubic centimeters Celsius degree") ;

    // J/(cm3.�C2) -> J/(m3.�K2)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatVolCapacityT,1.E+06) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatVolCapacityT,"J/(cm3.C2)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatVolCapacityT,"joule per cubic centimeters square Celsius degree") ;

    // 1/�C -> 1/�K
    m_mult_factors.setProperty(IUnitsSystemProperty::InvTemperature,1.) ;
    m_short_name.setProperty(IUnitsSystemProperty::InvTemperature,"1/C") ;
    m_long_name.setProperty(IUnitsSystemProperty::InvTemperature,"Reciprocal Celsius degree") ;

    // W/(m.�C) -> W/(m.K)
    m_mult_factors.setProperty(IUnitsSystemProperty::ThermalConductivity,1.) ;
    m_short_name.setProperty(IUnitsSystemProperty::ThermalConductivity,"W/(m.C)") ;
    m_long_name.setProperty(IUnitsSystemProperty::ThermalConductivity,"watts per meter Celsius degree") ;

    // CPO.M3/D/BAR -> PA.S.M3/S/PA
    m_mult_factors.setProperty(IUnitsSystemProperty::ProductivityIndex, 1.157407E-13) ;
    m_short_name.setProperty(IUnitsSystemProperty::ProductivityIndex,"cP.m3/D/bar") ;
    m_long_name.setProperty(IUnitsSystemProperty::ProductivityIndex,"Productivity Index unit") ;

    // J/D -> J/s
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatRate,11.57407e-06) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatRate,"J/D") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatRate,"joule per day") ;

    // J/g -> J/Kg
    m_mult_factors.setProperty(IUnitsSystemProperty::SpecificEnergy,1.E+3) ;
    m_short_name.setProperty(IUnitsSystemProperty::SpecificEnergy,"J/g") ;
    m_long_name.setProperty(IUnitsSystemProperty::SpecificEnergy,"joule per gram") ;

    // m3/d(cwe) -> dm3/s
    m_mult_factors.setProperty(IUnitsSystemProperty::SteamRate,1.157407E-2) ;
    m_short_name.setProperty(IUnitsSystemProperty::SteamRate,"m3/d(cwe)") ;
    m_long_name.setProperty(IUnitsSystemProperty::SteamRate,"Cold Water Equivalent Steam Flow") ;

    // m3 (cwe) -> dm3
    m_mult_factors.setProperty(IUnitsSystemProperty::SteamVolume,1000.) ;
    m_short_name.setProperty(IUnitsSystemProperty::SteamVolume,"m3(cwe)") ;
    m_long_name.setProperty(IUnitsSystemProperty::SteamVolume,"Cold Water Equivalent Steam Volume") ;

    // bar/m -> N/m3
    m_mult_factors.setProperty(IUnitsSystemProperty::Stiffness,1.E+5) ;
    m_short_name.setProperty(IUnitsSystemProperty::Stiffness,"bar/m") ;
    m_long_name.setProperty(IUnitsSystemProperty::Stiffness,"bar per meter") ;

    // mm/d -> m3/m2/s
    m_mult_factors.setProperty(IUnitsSystemProperty::RainFall,1.157407E-8) ;
    m_short_name.setProperty(IUnitsSystemProperty::RainFall,"mm/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::RainFall,"millimeter per day") ;

    // m2/d -> m2/s
    m_mult_factors.setProperty(IUnitsSystemProperty::ThermalDiffusivity,1.157407E-5) ;
    m_short_name.setProperty(IUnitsSystemProperty::ThermalDiffusivity,"m2/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::ThermalDiffusivity,"square meters per day") ;
}

bool
PMUUnitsSystemService::
hasMultFactor(IUnitsSystem::UnitType unit)
{
  return (m_mult_factors.hasProperty(unit)) ;
}

Real
PMUUnitsSystemService::
getMultFactor(IUnitsSystem::UnitType unit)
{
  return m_mult_factors.getProperty(unit,1.) ;
}

bool
PMUUnitsSystemService::
hasConstFactor(IUnitsSystem::UnitType unit)
{
  return (m_const_factors.hasProperty(unit)) ;
}

Real
PMUUnitsSystemService::
getConstFactor(IUnitsSystem::UnitType unit)
{
  return m_const_factors.getProperty(unit,0.) ;
}

bool
PMUUnitsSystemService::
hasMultFactorTo(IUnitsSystem* system, UnitType unit)
{
  return getMultFactor(unit) != system->getMultFactor(unit) ;
}

Real
PMUUnitsSystemService::
getMultFactorTo(IUnitsSystem* system, UnitType unit)
{
  return getMultFactor(unit)/system->getMultFactor(unit);
}

bool
PMUUnitsSystemService::
hasConstFactorTo(IUnitsSystem* system, UnitType unit)
{
  return getConstFactor(unit) != system->getConstFactor(unit);
}

Real
PMUUnitsSystemService::
getConstFactorTo(IUnitsSystem* system, UnitType unit)
{
  return (getConstFactor(unit)-system->getConstFactor(unit))/system->getMultFactor(unit);
}

Real PMUUnitsSystemService::convert(IUnitsSystem::UnitType unit,Real val)
{
  return m_const_factors.getProperty(unit,0.)+val*getMultFactor(unit) ;
}

Real PMUUnitsSystemService::unconvert(IUnitsSystem::UnitType unit,Real val)
{
  return (val-m_const_factors.getProperty(unit,0.))/getMultFactor(unit) ;
}

void PMUUnitsSystemService::convert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactor(unit) ;
  const Real const_factor = getConstFactor(unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

void PMUUnitsSystemService::unconvert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
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
PMUUnitsSystemService::
convertTo(IUnitsSystem* system, IUnitsSystem::UnitType unit,Real val)
{
  return getMultFactorTo(system,unit)*val + getConstFactorTo(system,unit);
}

void
PMUUnitsSystemService::
convertTo(IUnitsSystem* system,IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactorTo(system,unit) ;
  const Real const_factor = getConstFactorTo(system,unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

String
PMUUnitsSystemService::
getShortName(IUnitsSystem::UnitType unit)
{
  return m_short_name.getProperty(unit,IUnitsSystem::getSIUnitShortName(unit)) ;
}

String
PMUUnitsSystemService::
getLongName(IUnitsSystem::UnitType unit)
{
  return m_long_name.getProperty(unit,IUnitsSystem::getSIUnitLongName(unit)) ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_PMUUNITSSYSTEM(PMUUnitsSystem,PMUUnitsSystemService);

