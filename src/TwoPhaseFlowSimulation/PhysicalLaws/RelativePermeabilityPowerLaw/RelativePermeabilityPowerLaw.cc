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

//=====================================================

void RelativePermeabilityPowerLaw::eval( const Real Sw, Real& krw, Real& dkrw_dSw ) const
{   
  // Compute results  
  Real dSwBar_dSw;
  const Real SwBar   = computeSwBar( Sw, dSwBar_dSw );
  krw                 = pow(SwBar, m_alpha);
  dkrw_dSw = dSwBar_dSw * m_alpha * pow(SwBar, m_alpha - 1);
}

//=====================================================

Real RelativePermeabilityPowerLaw::computeSwBar( const Real Sw, Real& dSwBar_dSw ) const
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


//=====================================================

void RelativePermeabilityPowerLaw::setParameters( const Real Swi, const Real Sgc, const Real alpha  )
{
  // Set Parameters
  m_Swi = Swi;
  m_Sgc = Sgc;
  m_alpha = alpha;
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
