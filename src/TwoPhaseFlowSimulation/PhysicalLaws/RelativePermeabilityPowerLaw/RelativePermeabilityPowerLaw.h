// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * RelativePermeabilityPowerLaw.h
 */

#ifndef RELATIVEPERMEABILITYPOWERLAW_H_
#define RELATIVEPERMEABILITYPOWERLAW_H_

#include "ArcGeoSim/Utils/Utils.h"
#include <arcane/utils/ITraceMng.h>

//=====================================================

class RelativePermeabilityPowerLaw
{
public :
  
  // Constructeur Obligatoire
  RelativePermeabilityPowerLaw()
  {
    initParameters();
  }

  // Evaluation de la fonction et des derivees
  void eval( const Real Sw, Real& krw, Real& dkrw_dSw ) const;

  void setParameters( const Real Swi, const Real Sgc, const Real alpha );

private :

  void initParameters();

  Real computeSwBar( const Real Sw, Real& dSwBar_dSw ) const;

private :

  Real m_Swi;
  Real m_Sgc;
  Real m_alpha;
};

#endif /* RELATIVEPERMEABILITYPOWERLAW_H_ */
