// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * CapillaryPressureLaw.h
 */

#ifndef CAPILLARYPRESSURELAW_H_
#define CAPILLARYPRESSURELAW_H_

#include "ArcGeoSim/Utils/Utils.h"
#include <arcane/utils/ITraceMng.h>

//=====================================================

class CapillaryPressureLaw
{
public :

    // Constructeur Obligatoire
    CapillaryPressureLaw()
    {
        initParameters();
    }

    // Evaluation de la fonction et des derivees
    void eval( const Real S, Real& Pc, Real& dPc_dS ) const;

    void setParameters( const Real Pe, const Real Sr_ref, const Real Sr, const Real lambda );

private :

    void initParameters();

    Real computeSe( const Real S, Real& dSe_dS ) const;

private :

    Real m_Pe;
    Real m_Sr_ref;
    Real m_Sr;
    Real m_lambda;
};

#endif /* CAPILLARYPRESSURELAW_H_ */
