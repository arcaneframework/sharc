// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

/*
 * RelativePermeabilityPowerLaw.cc
 *  Lois Physiques Geoxim
 */

//-----------------------------------------------------------
// Loi de permeabilite relative de la phase aqueuse
// krw = krw (Sw)
//-----------------------------------------------------------

#include "RelativePermeabilityPowerLaw.h"
#include <immintrin.h>
//=====================================================

void RelativePermeabilityPowerLaw::eval_orig( const Real Sw, Real& krw, Real& dkrw_dSw ) const
{
    // Compute results
    Real dSwBar_dSw;
    const Real SwBar   = computeSwBar_orig( Sw, dSwBar_dSw );
    krw                 = pow(SwBar, m_alpha);
    dkrw_dSw = dSwBar_dSw * m_alpha * pow(SwBar, m_alpha - 1);
}

void RelativePermeabilityPowerLaw::eval( const Real Sw, Real& krw, Real& dkrw_dSw ) const
{   
  // Compute results  
  Real dSwBar_dSw;
  const Real SwBar   = computeSwBar( Sw, dSwBar_dSw );
  krw                 = pow(SwBar, m_alpha);
  dkrw_dSw = dSwBar_dSw * m_alpha * pow(SwBar, m_alpha - 1);
}

void RelativePermeabilityPowerLaw::evalVect( const mipp::Reg<Real> Sw, mipp::Reg<Real>& krw, mipp::Reg<Real>& dkrw_dSw ) const
{
    // Compute results
    mipp::Reg<Real> dSwBar_dSw;
    const mipp::Reg<Real> SwBar   = computeSwBarVect( Sw, dSwBar_dSw );
    Real tmp[mipp::N<Real>()];
    Real tmp2;
    SwBar.store(tmp);
    for (int i = 0; i < mipp::N<Real>(); i++) {
        tmp2 = pow(tmp[i], m_alpha - 1);
        tmp[i] = tmp2;
    }
    krw = mipp::load(tmp);
    dkrw_dSw = dSwBar_dSw * m_alpha * krw;
    krw *= SwBar;
}

//=====================================================
Real RelativePermeabilityPowerLaw::computeSwBar_orig( const Real Sw, Real& dSwBar_dSw ) const
{
  // Compute results
  Real SwBar;
  if ( Sw < m_Swi )
  {
    SwBar      = 0;
    dSwBar_dSw = 0;
  }
  else if ( Sw <= 1 - m_Sgc )
  {
    SwBar      = ( Sw - m_Swi ) / ( 1 - m_Sgc - m_Swi );
    dSwBar_dSw =              1 / ( 1 - m_Sgc - m_Swi );
  }
  else
  {
    SwBar      = 1;
    dSwBar_dSw = 0;
  }
  return SwBar;
}

Real RelativePermeabilityPowerLaw::computeSwBar( const Real Sw, Real& dSwBar_dSw ) const
{
    // Compute results

    Real SwBar = Sw - m_Swi;
    dSwBar_dSw = 1;
    if (SwBar < 0)
    {
        SwBar      = 0;
        dSwBar_dSw = 0;
    }

    SwBar      = SwBar * m_inv_w;
    dSwBar_dSw = dSwBar_dSw * m_inv_w;

    if (SwBar > 1)
    {
        SwBar      = 1;
        dSwBar_dSw = 0;
    }

    return SwBar;
}

mipp::Reg<Real> RelativePermeabilityPowerLaw::computeSwBarVect( const mipp::Reg<Real> Sw, mipp::Reg<Real>& dSwBar_dSw ) const
{
    // Compute results
    mipp::Reg<Real> SwBar = Sw - m_Swi;
    dSwBar_dSw = 1;
    mipp::Msk<mipp::N<Real>()>  k1 = mipp::cmplt<Real>(SwBar, mipp::set0<Real>());
    SwBar = mipp::blend<Real>(mipp::set0<Real>(), SwBar, k1);
    dSwBar_dSw = mipp::blend<Real>(mipp::set0<Real>(), dSwBar_dSw, k1);

    SwBar      = SwBar * m_inv_w;
    dSwBar_dSw = dSwBar_dSw * m_inv_w;

    k1 = mipp::cmpgt<Real>(SwBar, mipp::set1<Real>(1));
    SwBar = mipp::blend<Real>(mipp::set1<Real>(1), SwBar, k1);
    dSwBar_dSw = mipp::blend<Real>(mipp::set0<Real>(), dSwBar_dSw, k1);

    return SwBar;
}


//=====================================================

void RelativePermeabilityPowerLaw::setParameters( const Real Swi, const Real Sgc, const Real alpha  )
{
  // Set Parameters
  m_Swi = Swi;
  m_Sgc = Sgc;
  m_alpha = alpha;
  m_w = 1 - m_Sgc - m_Swi;
  m_inv_w = 1/m_w;
}

//=====================================================

void RelativePermeabilityPowerLaw::initParameters()
{
  // Init Parameters
  // Standard End Points
  const Real Swi   = 0;
  const Real Sgc   = 0;
  const Real alpha = 1;

  // Set Parameters
  setParameters( Swi, Sgc, alpha );
}
