// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * FluidDensityLaw.h
 */

#ifndef FLUIDDENSITYLAW_H_
#define FLUIDDENSITYLAW_H_

#include "ArcGeoSim/Utils/Utils.h"
#include <arcane/utils/ITraceMng.h>

//=====================================================

class FluidDensityLaw
{
public :

    // Constructeur Obligatoire
    FluidDensityLaw()
    {
        initParameters();
    }

    // Evaluation de la fonction et des derivees
    void eval( const Real T, const Real C, Real& rho, Real& drho_dt, Real& drho_dc) const;

    void setParameters( const Real rho0, const Real betat, const Real betac, const Real t0, const Real c0 );

private :

    void initParameters();

    //Real computeSe( const Real S, Real& dSe_dS ) const;

private :

    Real m_rho0;
    Real m_betat;
    Real m_betac;
    Real m_t0;
    Real m_c0;
};

#endif /* FLUIDDENSITYLAW_H_ */
