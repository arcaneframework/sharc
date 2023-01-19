// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

/*
 * PengRobinsonLaw.cc
 *  Lois Physiques
 */

//-----------------------------------------------------------
// Loi de densité de Peng-Robinson
// rho = 2*P*Sa
//-----------------------------------------------------------

#include "PengRobinsonLaw.h"

//=====================================================

void PengRobinsonLaw::eval( const Real P, const Real Sa, Real& rho, Real& drho_dP, Real& drho_dSa ) const
{
    // Compute results

    rho   = 2 * P * Sa;
    drho_dP = 2 * Sa;
    drho_dSa = 2 * P;
}
