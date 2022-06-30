// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef LAW_CONTRIBUTION_H
#define LAW_CONTRIBUTION_H

#include "ArcGeoSim/Physics/Law2/Contribution/Item.h"

#include "ArcGeoSim/Numerics/AuDi/audi.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_LAW_NAMESPACE

#ifdef SPARSE_AUDI
  using Contribution = audi::sparse_expression<Arcane::Real>;
  using RootContribution = audi::scalar_wrapper<Arcane::Real>;
#else
  using Contribution = audi::scalar<Arcane::Real>;
#endif
  using audi::one;

END_LAW_NAMESPACE
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* GEOXIM_NUMERICS_LINEARALGEBRA_CONTRIBUTION_H */
