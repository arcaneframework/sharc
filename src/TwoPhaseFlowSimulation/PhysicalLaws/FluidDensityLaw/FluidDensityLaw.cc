// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

/*
 * FluidDensityLaw.cc
 *  Lois Physiques
 */

//-----------------------------------------------------------
// Loi de la densité
//
//-----------------------------------------------------------

#include "FluidDensityLaw.h"

//=====================================================

void FluidDensityLaw::eval( const Real T, const Real C, Real& rho, Real& drho_dt, Real& drho_dc ) const
{
    // Compute results
    Real factor = (1.0 - m_betat*(T - m_t0) + m_betac*(C - m_c0));
    if(factor > 0)
    {
        rho = m_rho0 * factor;
        drho_dt = -1.0 * m_betat*m_rho0;
        drho_dc =  m_betac*m_rho0;
    }
    else {
        rho = m_rho0;
        drho_dt = 0.;    // A verifier
        drho_dc = 0.;    // A verifier
    }
}

//=====================================================
/*
Real FluidDensityLaw::computeSe( const Real S, Real& dSe_dS ) const
{

    Real Se = Arcane::math::min(1 - (S - m_Sr)/(1 - m_Sr_ref -m_Sr), 1.0);
    dSe_dS = - 1.0/(1 - m_Sr_ref -m_Sr);

    return Arcane::math::max(Se, 0.);

}
*/
//=====================================================

void FluidDensityLaw::setParameters( const Real rho0, const Real betat, const Real betac, const Real t0, const Real c0  )
{
    // Set Parameters

    m_rho0 = rho0;
    m_betat = betat;
    m_betac = betac;
    m_t0 = t0;
    m_c0 = c0;

}

//=====================================================

void FluidDensityLaw::initParameters()
{
    // Init Parameters
    // Standard End Points
    const Real rho0    = 0;
    const Real betat   = 0;
    const Real betac   = 0;
    const Real t0 = 0;
    const Real c0 = 0;

    // Set Parameters
    setParameters( rho0, betat, betac, t0, c0 );
}
