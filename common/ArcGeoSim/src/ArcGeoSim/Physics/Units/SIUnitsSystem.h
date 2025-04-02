// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef SIUNITSSYSTEM_H
#define SIUNITSSYSTEM_H

#include "IUnitsSystem.h"
#include "SIUnitsSystem_axl.h"

using namespace Arcane ;

/**
 * Interface du service du modu?le de resolution non lineaire.
 */

class SIUnitsSystemService :
    public ArcaneSIUnitsSystemObject
{
public:
  /** Constructeur de la classe */
  SIUnitsSystemService(const ServiceBuildInfo & sbi) 
  : ArcaneSIUnitsSystemObject(sbi) 
  {
  }
  
  /** Destructeur de la classe */
  virtual ~SIUnitsSystemService() {};
  
public:
  void init() {}
   
  //! @name D�termination de coefficient vers SI
  //@{
  bool hasMultFactor(UnitType unit) { return false ; }
  Real getMultFactor(UnitType unit) { return 1. ; }
  bool hasConstFactor(UnitType unit) { return false ; }
  Real getConstFactor(UnitType unit) { return 0. ; }
  //@}

  //! @name D�termination de coefficient vers syst�me sp�cifique
  //@{
  bool hasMultFactorTo(IUnitsSystem* system, UnitType unit) { return system->hasMultFactor(unit); }
  Real getMultFactorTo(IUnitsSystem* system, UnitType unit) { return 1./system->getMultFactor(unit); }
  bool hasConstFactorTo(IUnitsSystem* system, UnitType unit) { return system->hasConstFactor(unit); }
  Real getConstFactorTo(IUnitsSystem* system, UnitType unit) { return -system->getConstFactor(unit)/system->getMultFactor(unit); }
  //@}

  //! @name Fonctions de conversion
  //@{
  Real convert(UnitType unit,Real val) { return val ; }
  Real unconvert(UnitType unit,Real val) { return val ; }
  Real convertTo(IUnitsSystem* system,UnitType unit,Real val);
  void convert(UnitType unit,ArrayView<Real> buf) { return ; }
  void unconvert(UnitType unit,ArrayView<Real> buf) { return ; }
  void convertTo(IUnitsSystem* system,UnitType unit,ArrayView<Real> buf) ;
  //@}
private :
  
};

//END_NAME_SPACE_PROJECT

#endif
