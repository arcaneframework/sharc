// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*
 * PengRobinsonLaw.h
 */

#ifndef PENGROBINSONLAW_H_
#define PENGROBINSONLAW_H_

#include "ArcGeoSim/Utils/Utils.h"
#include <arcane/utils/ITraceMng.h>

//=====================================================

class PengRobinsonLaw
{
public :

    // Constructeur Obligatoire
    PengRobinsonLaw() {}

    // Evaluation de la fonction et des derivees

    void eval( const Real P, const Real Sa, Real& rho, Real& drho_dP, Real& drho_dSa ) const;

};

#endif /* PENGROBINSONLAW_H_ */
