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
#include <mipp.h>

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
  void eval_orig( const Real Sw, Real& krw, Real& dkrw_dSw ) const;
  void eval( const Real Sw, Real& krw, Real& dkrw_dSw ) const;
  void evalVect( const mipp::Reg<Real> Sw, mipp::Reg<Real>& krw, mipp::Reg<Real>& dkrw_dSw ) const;

  void setParameters( const Real Swi, const Real Sgc, const Real alpha );

private :

  void initParameters();

  Real computeSwBar_orig( const Real Sw, Real& dSwBar_dSw ) const;
  Real computeSwBar( const Real Sw, Real& dSwBar_dSw ) const;
  mipp::Reg<Real> computeSwBarVect( const mipp::Reg<Real> Sw, mipp::Reg<Real>& dSwBar_dSw ) const;

private :

  Real m_Swi;
  Real m_Sgc;
  Real m_alpha;
  Real m_w;
  Real m_inv_w;
};

#endif /* RELATIVEPERMEABILITYPOWERLAW_H_ */
