// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/PropertyHolder.h"
#include "IUnitsSystem.h"
#include "PFUUnitsSystem.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
PFUUnitsSystemService::
init()
{
	// jour -> seconde
    m_mult_factors.setProperty(IUnitsSystemProperty::Time,86400.) ;
    m_short_name.setProperty(IUnitsSystemProperty::Time,"D.") ;
    m_long_name.setProperty(IUnitsSystemProperty::Time,"day") ;
	// ft -> m
    m_mult_factors.setProperty(IUnitsSystemProperty::X,0.3048) ;
    m_short_name.setProperty(IUnitsSystemProperty::X,"ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::X,"feet") ;

    m_mult_factors.setProperty(IUnitsSystemProperty::Y,0.3048) ;
    m_short_name.setProperty(IUnitsSystemProperty::Y,"ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::Y,"feet") ;

    m_mult_factors.setProperty(IUnitsSystemProperty::Z,0.3048) ;
    m_short_name.setProperty(IUnitsSystemProperty::Z,"ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::Z,"feet") ;

    // rad -> deg
    m_mult_factors.setProperty(IUnitsSystemProperty::Angle,0.01745329) ;
    m_short_name.setProperty(IUnitsSystemProperty::Angle,"deg") ;
    m_long_name.setProperty(IUnitsSystemProperty::Angle,"degree") ;

    // ft2 -> m2
    m_mult_factors.setProperty(IUnitsSystemProperty::Surface,0.092903040) ;
    m_short_name.setProperty(IUnitsSystemProperty::Surface,"ft2") ;
    m_long_name.setProperty(IUnitsSystemProperty::Surface,"square feet") ;

    // ft -> m
    m_mult_factors.setProperty(IUnitsSystemProperty::Height,0.3048) ;
    m_short_name.setProperty(IUnitsSystemProperty::Height,"ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::Height,"feet") ;

    // BBl -> m3
    m_mult_factors.setProperty(IUnitsSystemProperty::Volume,0.1589873) ;
    m_short_name.setProperty(IUnitsSystemProperty::Volume,"BBl") ;
    m_long_name.setProperty(IUnitsSystemProperty::Volume,"Barils per day") ;

    // ft3 -> m3
    m_mult_factors.setProperty(IUnitsSystemProperty::GasVolume,0.0283168) ;
    m_short_name.setProperty(IUnitsSystemProperty::GasVolume,"ft3") ;
    m_long_name.setProperty(IUnitsSystemProperty::GasVolume,"cubic feet") ;

    // BBl/Mcft -> vol/vol
    m_mult_factors.setProperty(IUnitsSystemProperty::GasVolumeFactor,0.00561) ;
    m_short_name.setProperty(IUnitsSystemProperty::GasVolumeFactor,"BBl/mcft") ;
    m_long_name.setProperty(IUnitsSystemProperty::GasVolumeFactor,"Barils per 1E3 cubic feet") ;

    // psi -> pa
    m_mult_factors.setProperty(IUnitsSystemProperty::Pressure,6.894757E+03) ;
    m_short_name.setProperty(IUnitsSystemProperty::Pressure,"psi") ;
    m_long_name.setProperty(IUnitsSystemProperty::Pressure,"pound per square inch") ;

    m_mult_factors.setProperty(IUnitsSystemProperty::CapillaryPressure,6.894757E+03) ;
    m_short_name.setProperty(IUnitsSystemProperty::CapillaryPressure,"psi") ;
    m_long_name.setProperty(IUnitsSystemProperty::CapillaryPressure,"pound per square inch") ;

    // lb/ft2 -> N/m2
    m_mult_factors.setProperty(IUnitsSystemProperty::Constraint,4.8824) ;
    m_short_name.setProperty(IUnitsSystemProperty::Constraint,"psi") ;
    m_long_name.setProperty(IUnitsSystemProperty::Constraint,"pound per square inch") ;

    // d�F -> d�K
    m_mult_factors.setProperty(IUnitsSystemProperty::Temperature,.5555555) ;
    m_const_factors.setProperty(IUnitsSystemProperty::Temperature,255.3722) ;
    m_short_name.setProperty(IUnitsSystemProperty::Temperature,"F") ;
    m_long_name.setProperty(IUnitsSystemProperty::Temperature,"Fahrenheit degree") ;

    // F/ft -> K/m
    m_mult_factors.setProperty(IUnitsSystemProperty::ThermalGradient,1.8226870) ;
    m_short_name.setProperty(IUnitsSystemProperty::ThermalGradient,"F/ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::ThermalGradient,"Fahrenheit degree per feet") ;

    // md.ft -> m3
    m_mult_factors.setProperty(IUnitsSystemProperty::Transmissivity,3.0081E-16) ;
    m_short_name.setProperty(IUnitsSystemProperty::Transmissivity,"md.ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::Transmissivity,"md.ft") ;

    // md -> m2
    m_mult_factors.setProperty(IUnitsSystemProperty::Permeability,9.869233E-16) ;
    m_short_name.setProperty(IUnitsSystemProperty::Permeability,"md") ;
    m_long_name.setProperty(IUnitsSystemProperty::Permeability,"milli darcy") ;

    // ft2/d -> m2/s
    m_mult_factors.setProperty(IUnitsSystemProperty::Diffusion,1.07527E-6) ;
    m_short_name.setProperty(IUnitsSystemProperty::Diffusion,"ft2/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::Diffusion,"square foot per day") ;

    // 1/d -> 1/s
    m_mult_factors.setProperty(IUnitsSystemProperty::Flow,.1157407E-4) ;
    m_short_name.setProperty(IUnitsSystemProperty::Flow,"mol/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::Flow,"mole per day") ;

	// MMlb/d -> kg/s
    m_mult_factors.setProperty(IUnitsSystemProperty::MassFlow,5.249907) ;
    m_short_name.setProperty(IUnitsSystemProperty::MassFlow,"MMlb/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::MassFlow,"MMlb/d") ;

    // D�bit liquide (bbl/d) -> m3/s
    m_mult_factors.setProperty(IUnitsSystemProperty::LiqVolFlow,1.8401E-6) ;
    m_short_name.setProperty(IUnitsSystemProperty::LiqVolFlow,"BBl/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::LiqVolFlow,"Barils per day") ;

    // D�bit gas (Mcf/d) -> m3/s
    m_mult_factors.setProperty(IUnitsSystemProperty::GasVolFlow,3.2774E-4) ;
    m_short_name.setProperty(IUnitsSystemProperty::GasVolFlow,"Mcf/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::GasVolFlow,"1000*cft per day") ;

    // mol/ft3 -> mol/m3
    //m_mult_factors.setProperty(IUnitsSystemProperty::Density,16.0185) ;

    // cP -> Pa/s
    m_mult_factors.setProperty(IUnitsSystemProperty::Viscosity,0.001) ;
    m_short_name.setProperty(IUnitsSystemProperty::Viscosity,"cP") ;
    m_long_name.setProperty(IUnitsSystemProperty::Viscosity,"centiPoise") ;

    // 1/psi -> 1/pa
    m_mult_factors.setProperty(IUnitsSystemProperty::Compressibility,1.45038E-4) ;
    m_short_name.setProperty(IUnitsSystemProperty::Compressibility,"1/psi") ;
    m_long_name.setProperty(IUnitsSystemProperty::Compressibility,"square inch per pound") ;

    m_mult_factors.setProperty(IUnitsSystemProperty::Mass,.453592E+6) ;
    m_short_name.setProperty(IUnitsSystemProperty::Mass,"MMlb") ;
    m_long_name.setProperty(IUnitsSystemProperty::Mass,"MMlb") ;

    // lb/lbmole-> kg/mol
    m_mult_factors.setProperty(IUnitsSystemProperty::MolarMass,0.001) ;
    m_short_name.setProperty(IUnitsSystemProperty::MolarMass,"g/mole") ;
    m_long_name.setProperty(IUnitsSystemProperty::MolarMass,"grammme per mole") ;

    // cft/lbmole -> m3/mol
    m_mult_factors.setProperty(IUnitsSystemProperty::MolarVol,0.6242796E-4) ;
    m_short_name.setProperty(IUnitsSystemProperty::MolarVol,"cft.lbmol-1") ;
    m_long_name.setProperty(IUnitsSystemProperty::MolarVol,"cubic feet per lbmole") ;

    // lb/ft3 -> kg/m3
    m_mult_factors.setProperty(IUnitsSystemProperty::MassDensity,16.0185) ;
    m_short_name.setProperty(IUnitsSystemProperty::MassDensity,"lb/ft3") ;
    m_long_name.setProperty(IUnitsSystemProperty::MassDensity,"lb per cubic feet") ;

    // cft/bbl -> vol/vol
    m_mult_factors.setProperty(IUnitsSystemProperty::Rs,0.178108) ;
    m_short_name.setProperty(IUnitsSystemProperty::Rs,"cft/bbl") ;
    m_long_name.setProperty(IUnitsSystemProperty::Rs,"cubic feet per barrel") ;

    // Mscft/bbl -> vol/vol
    m_mult_factors.setProperty(IUnitsSystemProperty::GasOilVolumeRatio,178.108) ;
    m_short_name.setProperty(IUnitsSystemProperty::Rs,"Mscft/bbl") ;
    m_long_name.setProperty(IUnitsSystemProperty::Rs,"10E3 standard cubic feet per barrel") ;


        // inch -> m
    m_mult_factors.setProperty(IUnitsSystemProperty::Radius,0.0254) ;
    m_short_name.setProperty(IUnitsSystemProperty::Radius,"inch") ;
    m_long_name.setProperty(IUnitsSystemProperty::Radius,"inch") ;

    // Btu -> J
    m_mult_factors.setProperty(IUnitsSystemProperty::Energy,1055.056) ;
    m_short_name.setProperty(IUnitsSystemProperty::Energy,"Btu") ;
    m_long_name.setProperty(IUnitsSystemProperty::Energy,"British thermal unit") ;

    // Btu/(lb.�F) -> J/(Kg.�K)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatMassCapacity,4.1868E+3) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatMassCapacity,"Btu/(lb.F)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatMassCapacity,"British thermal unit per pound Fahrenheit degree") ;

    // Btu/(lb.�F2) -> J/(Kg.�K2)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatMassCapacityT,7.5362E+3) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatMassCapacityT,"Btu/(lb.F2)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatMassCapacityT,"British thermal unit per pound square Fahrenheit degree") ;

    // Btu/(cft.�F) -> J/(m3.�K)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatVolCapacity,6.7066E+4) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatVolCapacity,"Btu/(cft.F)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatVolCapacity,"British thermal unit per cubic feet Fahrenheit degree") ;

    // Btu/(cft.�F2) -> J/(m3.�K2)
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatVolCapacityT,1.2071861E+5) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatVolCapacityT,"Btu/(cft.F2)") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatVolCapacityT,"British thermal unit per cubic feet square Fahrenheit degree") ;

    // 1/�F -> 1/�K
    m_mult_factors.setProperty(IUnitsSystemProperty::InvTemperature,1.8) ;
    m_short_name.setProperty(IUnitsSystemProperty::InvTemperature,"1/F") ;
    m_long_name.setProperty(IUnitsSystemProperty::InvTemperature,"Reciprocal Fahrenheit degree") ;

    // Btu/(ft.D.�F) -> W/(m.K)
    m_mult_factors.setProperty(IUnitsSystemProperty::ThermalConductivity,0.072114) ;
    m_short_name.setProperty(IUnitsSystemProperty::ThermalConductivity,"Btu/(ft.D.F)") ;
    m_long_name.setProperty(IUnitsSystemProperty::ThermalConductivity,"British thermal unit per day feet Fahrenheit degree") ;

    // CPO.BBL/D/PSI -> PA.S.M3/S/PA
    m_mult_factors.setProperty(IUnitsSystemProperty::ProductivityIndex, 2.6688E-13) ;
    m_short_name.setProperty(IUnitsSystemProperty::ProductivityIndex,"cP.BBl/D/psi") ;
    m_long_name.setProperty(IUnitsSystemProperty::ProductivityIndex,"Productivity Index unit") ;

    // Btu/D -> J/s
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatRate,12211.292e-06) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatRate,"Btu/D") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatRate,"British thermal unit per day") ;

    // Btu/lb -> J/Kg
    m_mult_factors.setProperty(IUnitsSystemProperty::SpecificEnergy,2.3260E+3) ;
    m_short_name.setProperty(IUnitsSystemProperty::SpecificEnergy,"Btu/lb") ;
    m_long_name.setProperty(IUnitsSystemProperty::SpecificEnergy,"British thermal unit per pound") ;

    // BBl/D(cwe) -> dm3/s
    m_mult_factors.setProperty(IUnitsSystemProperty::SteamRate,1.8401E-3) ;
    m_short_name.setProperty(IUnitsSystemProperty::SteamRate,"BBl/d(cwe)") ;
    m_long_name.setProperty(IUnitsSystemProperty::SteamRate,"British Cold Water Equivalent Steam Flow") ;

    // BBl(cwe) -> dm3
    m_mult_factors.setProperty(IUnitsSystemProperty::SteamVolume,158.9873) ;
    m_short_name.setProperty(IUnitsSystemProperty::SteamVolume,"BBl(cwe)") ;
    m_long_name.setProperty(IUnitsSystemProperty::SteamVolume,"British Cold Water Equivalent Steam Volume") ;

    // Mcft/t -> m3/kg
    m_mult_factors.setProperty(IUnitsSystemProperty::MassicVolume,0.02831684659) ;
    m_short_name.setProperty(IUnitsSystemProperty::MassicVolume,"mcft/t") ;
    m_long_name.setProperty(IUnitsSystemProperty::MassicVolume,"1E3 cubic feet per ton") ;

    // aft3 - > m3
    m_mult_factors.setProperty(IUnitsSystemProperty::PorousVolume,1233.489) ;
    m_short_name.setProperty(IUnitsSystemProperty::PorousVolume,"aft3") ;
    m_long_name.setProperty(IUnitsSystemProperty::PorousVolume,"Acre feet") ;

    // psi/ft -> N/m3
    m_mult_factors.setProperty(IUnitsSystemProperty::Stiffness,2.262059E+4) ;
    m_short_name.setProperty(IUnitsSystemProperty::Stiffness,"psi/ft") ;
    m_long_name.setProperty(IUnitsSystemProperty::Stiffness,"psi per feet") ;

    // Btu/(D.ft2) -> W/m2
    m_mult_factors.setProperty(IUnitsSystemProperty::HeatFluxDensity,0.1314406) ;
    m_short_name.setProperty(IUnitsSystemProperty::HeatFluxDensity,"Btu/D/ft2") ;
    m_long_name.setProperty(IUnitsSystemProperty::HeatFluxDensity,"British Thermal Unit per day per square foot") ;

    // inch/d -> m3/m2/s
    m_mult_factors.setProperty(IUnitsSystemProperty::RainFall,2.93981481e-7) ;
    m_short_name.setProperty(IUnitsSystemProperty::RainFall,"inch/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::RainFall,"inch per day") ;

    // ft2/d -> m2/s
    m_mult_factors.setProperty(IUnitsSystemProperty::ThermalDiffusivity,1.07527E-6) ;
    m_short_name.setProperty(IUnitsSystemProperty::ThermalDiffusivity,"ft2/d") ;
    m_long_name.setProperty(IUnitsSystemProperty::ThermalDiffusivity,"square feet per day") ;
}

bool
PFUUnitsSystemService::
hasMultFactor(IUnitsSystem::UnitType unit)
{
  return (m_mult_factors.hasProperty(unit)) ;
}

Real
PFUUnitsSystemService::
getMultFactor(IUnitsSystem::UnitType unit)
{
  return m_mult_factors.getProperty(unit,1.) ;
}

bool
PFUUnitsSystemService::
hasConstFactor(IUnitsSystem::UnitType unit)
{
  return (m_const_factors.hasProperty(unit)) ;
}

Real
PFUUnitsSystemService::
getConstFactor(IUnitsSystem::UnitType unit)
{
  return m_const_factors.getProperty(unit,0.) ;
}

bool
PFUUnitsSystemService::
hasMultFactorTo(IUnitsSystem* system, UnitType unit)
{
  return getMultFactor(unit) != system->getMultFactor(unit) ;
}

Real
PFUUnitsSystemService::
getMultFactorTo(IUnitsSystem* system, UnitType unit)
{
  return getMultFactor(unit)/system->getMultFactor(unit);
}

bool
PFUUnitsSystemService::
hasConstFactorTo(IUnitsSystem* system, UnitType unit)
{
  return getConstFactor(unit) != system->getConstFactor(unit);
}

Real
PFUUnitsSystemService::
getConstFactorTo(IUnitsSystem* system, UnitType unit)
{
  return (getConstFactor(unit)-system->getConstFactor(unit))/system->getMultFactor(unit);
}

Real PFUUnitsSystemService::convert(IUnitsSystem::UnitType unit,Real val)
{
  return m_const_factors.getProperty(unit,0.)+val*getMultFactor(unit) ;
}

Real PFUUnitsSystemService::unconvert(IUnitsSystem::UnitType unit,Real val)
{
  return (val-m_const_factors.getProperty(unit,0.))/getMultFactor(unit) ;
}

void PFUUnitsSystemService::convert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactor(unit) ;
  const Real const_factor = getConstFactor(unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

void PFUUnitsSystemService::unconvert(IUnitsSystem::UnitType unit,ArrayView<Real> buf)
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
PFUUnitsSystemService::
convertTo(IUnitsSystem* system, IUnitsSystem::UnitType unit,Real val)
{
  return getMultFactorTo(system,unit)*val + getConstFactorTo(system,unit);
}

void
PFUUnitsSystemService::
convertTo(IUnitsSystem* system,IUnitsSystem::UnitType unit,ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactorTo(system,unit) ;
  const Real const_factor = getConstFactorTo(system,unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

String
PFUUnitsSystemService::
getShortName(IUnitsSystem::UnitType unit)
{
  return m_short_name.getProperty(unit,IUnitsSystem::getSIUnitShortName(unit)) ;
}

String
PFUUnitsSystemService::
getLongName(IUnitsSystem::UnitType unit)
{
  return m_long_name.getProperty(unit,IUnitsSystem::getSIUnitLongName(unit)) ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_PFUUNITSSYSTEM(PFUUnitsSystem,PFUUnitsSystemService);

