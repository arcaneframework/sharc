// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef UNITSSYSTEM_H
#define UNITSSYSTEM_H

#include "IUnitsSystem.h"
#include "UnitsSystem_axl.h"

using namespace Arcane ;

/**
 * Service d'unit� utilisateur
 */

class UnitsSystemService :
    public ArcaneUnitsSystemObject
{
public:
  /** Constructeur de la classe */
  UnitsSystemService(const ServiceBuildInfo & sbi) 
  : ArcaneUnitsSystemObject(sbi) 
  {
    m_mult_factors.setName("MultFactors") ;
    m_mult_factors.setDefaultValue(1.) ;
  }
  
  /** Destructeur de la classe */
  virtual ~UnitsSystemService() {};
  
public:
  void init() ;
   
  //! @name D�termination de coefficient vers SI
  //@{
  bool hasMultFactor(UnitType unit)
  {  return (m_mult_factors.hasProperty(unit)) ; }

  Real getMultFactor(UnitType unit)
  { return m_mult_factors.getProperty(unit,1.) ; }
  
  bool hasConstFactor(UnitType unit)
  {  return (m_const_factors.hasProperty(unit)) ; }

  Real getConstFactor(UnitType unit)
  { return m_const_factors.getProperty(unit,0.) ; }
  //@}
  
  //! @name D�termination de coefficient vers syst�me sp�cifique
  //@{
  bool hasMultFactorTo(IUnitsSystem* system, UnitType unit);
  Real getMultFactorTo(IUnitsSystem* system, UnitType unit);
  bool hasConstFactorTo(IUnitsSystem* system, UnitType unit);
  Real getConstFactorTo(IUnitsSystem* system, UnitType unit);
  //@}

  //! @name Fonctions de conversion
  //@{
  Real convert(UnitType unit,Real val) { return val*getMultFactor(unit)+getConstFactor(unit) ; }
  Real unconvert(UnitType unit,Real val) { return (val-getConstFactor(unit))/getMultFactor(unit) ; }
  Real convertTo(IUnitsSystem* system,UnitType unit,Real val) ;
  void convert(UnitType unit,ArrayView<Real> buf) ;
  void unconvert(UnitType unit,ArrayView<Real> buf) ;
  void convertTo(IUnitsSystem* system,UnitType unit,ArrayView<Real> buf) ;
  //@}
  
  String getShortName(UnitType unit) ;
  String getLongName(UnitType unit) ;
private :
  
  PropertyHolder<IUnitsSystemProperty,Real> m_mult_factors ;
  PropertyHolder<IUnitsSystemProperty,Real> m_const_factors ;
  PropertyHolder<IUnitsSystemProperty,String> m_short_name ;
  PropertyHolder<IUnitsSystemProperty,String> m_long_name ;
};

//END_NAME_SPACE_PROJECT

#endif
