// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef IUNITSSYSTEM_H
#define IUNITSSYSTEM_H

#include <arcane/ArcaneTypes.h>
#include <arcane/utils/String.h>
#include <utility>

using namespace Arcane;

/**
 * Interface du service de gestion des unit�s
 * Suppose pour l'instant que les conversions sont affines
 */
typedef std::pair<String,String> UnitNames ;
const UnitNames
si_unit_name[] = {
  UnitNames("u", "unit"),
  UnitNames("s", "second"),
  UnitNames("m", "meter"),
  UnitNames("m", "meter"),
  UnitNames("m", "meter"),
  UnitNames("rad", "radian"),
  UnitNames("m2", "square meter"),
  UnitNames("m", "meter"),
  UnitNames("m3", "cubic meters"),
  UnitNames("m3", "cubic meters"),
  UnitNames("vol/vol", "gas formation volume factor unit"),
  UnitNames("Pa", "Pascal"),
  UnitNames("Pa", "Pascal"),
  UnitNames("N/m2", "Newton per square meter"),
  UnitNames("K", "Kelvin degree"),
  UnitNames("1/K", "Reciprocal Kelvin degree"),
  UnitNames("K/m", "Kelvin degree per meter"),
  UnitNames("Trans unit", "Trans unit"),
  UnitNames("Porosity unit", "Porosity unit"),
  UnitNames("Porous Volume unit", "Porous Volume unit"),
  UnitNames("Permeability unit", "Permeability unit"),
  UnitNames("m2", "Dispertion unit"),
  UnitNames("m2/s", "Diffusion unit"),
  UnitNames("u.s-1", "Flow unit"),
  UnitNames("Kg/s", "MassFlow unit"),
  UnitNames("m3/s", "LiqVolFlow unit"),
  UnitNames("m3/s", "GasVolFlow unit"),
  UnitNames("Kg/m3", "Kilogramme per cubic meters"),
  UnitNames("Pa.s", "Viscosity unit"),
  UnitNames("1/Pa", "Compressibility unit"),
  UnitNames("Kg", "Weight unit"),
  UnitNames("Kg", "Kilogramme"),
  UnitNames("Kg/mol", "Kilogramme per mole"),
  UnitNames("Kg/m3", "Kilogramme per cubic meters"),
  UnitNames("m3/mol", "cubic meters per mole"),
  UnitNames("mol/m3", "Mole per cubic meters"),
  UnitNames("sqrt(J/m3)", "sqrt(Joule per cubic meters)"),
  UnitNames("mol/Kg", "mole per Kg"),
  UnitNames("g/l", "gramme per liter"),
  UnitNames("Kg", "Kilogramme"),
  UnitNames("m3/m3", "cubique meter per cubic meters"),
  UnitNames("m", "meter"),
  UnitNames("J", "Joule"),
  UnitNames("J/(Kg.K)", "Joule per kilogramme Kelvin"),
  UnitNames("J/(Kg.K2)", "Joule per kilogramme square Kelvin"),
  UnitNames("J/(m3.K)", "Joule per cubic meter Kelvin"),
  UnitNames("J/(m3.K2)", "Joule per cubic meter square Kelvin"),
  UnitNames("W/(m.K)", "watts per meter Kelvin "),
  UnitNames("m2/s", "square meters per second"),
  UnitNames("Pa.s.m3/s/Pa", "Productivity Index unit"),
  UnitNames("J/s", "Joule per second"),
  UnitNames("J/Kg", "Joule per kilogramme"),
  UnitNames("dm3/s(cwe)", "Cold Water Equivalent Steam Flow"),
  UnitNames("dm3(cwe)", "Cold Water Equivalent Steam Volume"),
  UnitNames("m3/kg", "volume gas per mass of unit"),
  UnitNames("N/m3", "force per volume"),
  UnitNames("W/m2", "watts per square meters"),
  UnitNames("m3/m2/s", "cubic meter per square meters and per second"),
  UnitNames("Undefined unit", "Undefined unit")
};

class IUnitsSystemProperty
{
public:
  typedef enum { Adim,
                 Time,
                 X,
                 Y,
                 Z,
                 Angle,
                 Surface,
                 Height,
                 Volume,
                 GasVolume,
                 GasVolumeFactor,
                 Pressure,
                 CapillaryPressure,
                 Constraint,
                 Temperature,
                 InvTemperature,
                 ThermalGradient,
                 Transmissivity,
                 Porosity,
                 PorousVolume,
                 Permeability,
                 Dispertion,
                 Diffusion,
                 Flow,
                 MassFlow,
                 LiqVolFlow,
                 GasVolFlow,
                 Density,
                 Viscosity,
                 Compressibility,
                 Weight,
                 Mass,
                 MolarMass,
                 MassDensity,
                 MolarVol,
                 Concentration,
                 Solubility,
                 Salinity,
                 Production,
                 MassProduction,
                 Rs,
                 GasOilVolumeRatio,
                 Radius,
                 Energy,
                 HeatMassCapacity,
                 HeatMassCapacityT,
                 HeatVolCapacity,
                 HeatVolCapacityT,
                 ThermalConductivity,
                 ThermalDiffusivity,
                 ProductivityIndex,
                 HeatRate,
                 SpecificEnergy,
                 SteamRate,
                 SteamVolume,
                 MassicVolume,
                 Stiffness,
                 HeatFluxDensity,
                 RainFall,
                 Unknown
                } eProperty ;
} ;

class IUnitsSystem
{
public:
  //! Constructeur de la classe
  IUnitsSystem() {};

  //! Destructeur de la classe
  virtual ~IUnitsSystem() {};

public:
  //! Initialisation
  virtual void init() = 0 ;

  //! Rappel du type d'unit�
  typedef IUnitsSystemProperty::eProperty UnitType ;

  //! Existence d'un facteur pour passer en unit� standart (local->SI)
  virtual bool hasMultFactor(UnitType unit) = 0 ;
  //! Facteur pour passer en unit� standart (local->SI)
  virtual Real getMultFactor(UnitType unit) = 0 ;
  //! Existence d'un terme pour passer en unit� standart (local->SI)
  virtual bool hasConstFactor(UnitType unit) = 0 ;
  //! Terme pour passer en unit� standart (local->SI)
  virtual Real getConstFactor(UnitType unit) = 0 ;

  //! Existence d'un facteur pour passer dans un autre syst�me (local->system)
  virtual bool hasMultFactorTo(IUnitsSystem* system, UnitType unit) = 0 ;
  //! Facteur pour passer dans un autre syst�me (local->system)
  virtual Real getMultFactorTo(IUnitsSystem* system, UnitType unit) = 0 ;
  //! Existence d'un terme pour passer dans un autre syst�me (local->system)
  virtual bool hasConstFactorTo(IUnitsSystem* system, UnitType unit) = 0 ;
  //! Terme pour passer dans un autre syst�me (local->system)
  virtual Real getConstFactorTo(IUnitsSystem* system, UnitType unit) = 0 ;

  //! Conversion en SI
  /*! \param val est suppos� en unit� locale */
  virtual Real convert(UnitType unit,Real val) = 0 ;
  //! inverse de convert
  //! Conversion en locale � partir de SI
  /*! \param val est suppos� en unit� SI */
  virtual Real unconvert(UnitType unit,Real val) = 0 ;
  //! Conversion dans un syst�me d'unit� donn�
  /*! \param val est suppos� en unit� locale */
  virtual Real convertTo(IUnitsSystem* system,UnitType unit,Real val) = 0 ;
  //! Conversion en SI
  /*! \param val est suppos� en unit� locale */
  virtual void convert(UnitType unit,ArrayView<Real> buf) = 0 ;
  //! inverse de convert
  //! Conversion en locale � partir de SI
  /*! \param val est suppos� en unit� SI */
  virtual void unconvert(UnitType unit,ArrayView<Real> buf) = 0 ;
  //! Conversion dans un syst�me d'unit� donn�
  /*! \param val est suppos� en unit� locale */
  virtual void convertTo(IUnitsSystem* system,UnitType unit,ArrayView<Real> buf) = 0 ;

  static String getSIUnitShortName(UnitType unit)
  { return si_unit_name[unit].first ; }
  static String getSIUnitLongName(UnitType unit)
  { return si_unit_name[unit].second ; }

  virtual String getShortName(UnitType unit)
  { return si_unit_name[unit].first ; }
  virtual String getLongName(UnitType unit)
  { return si_unit_name[unit].second ; }
};


#endif
