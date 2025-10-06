// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef PAUUNITSSYSTEM_H
#define PAUUNITSSYSTEM_H

#include "IUnitsSystem.h"
#include "PAUUnitsSystem_axl.h"

using namespace Arcane ;

/**
 * Interface du service du modu?le de resolution non lineaire.
 */

class PAUUnitsSystemService :
    public ArcanePAUUnitsSystemObject
{
public:
  /** Constructeur de la classe */
  PAUUnitsSystemService(const ServiceBuildInfo & sbi) 
  : ArcanePAUUnitsSystemObject(sbi) 
  {
    m_mult_factors.setName("MultFactors") ;
    m_mult_factors.setDefaultValue(1.) ;
  }
  
  /** Destructeur de la classe */
  virtual ~PAUUnitsSystemService() {};
  
public:
  void init() ;
   
  //! @name D�termination de coefficient vers SI
  //@{
  bool hasMultFactor(UnitType unit) ;
  Real getMultFactor(UnitType unit) ;
  bool hasConstFactor(UnitType unit) ;
  Real getConstFactor(UnitType unit) ;
  //@}

  //! @name D�termination de coefficient vers syst�me sp�cifique
  //@{
  bool hasMultFactorTo(IUnitsSystem* system, UnitType unit) ;
  Real getMultFactorTo(IUnitsSystem* system, UnitType unit) ;
  bool hasConstFactorTo(IUnitsSystem* system, UnitType unit) ;
  Real getConstFactorTo(IUnitsSystem* system, UnitType unit) ;
  //@}

  //! @name Fonctions de conversion
  //@{
  Real convert(UnitType unit,Real val) ;
  Real unconvert(UnitType unit,Real val) ;
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
