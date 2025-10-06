// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#pragma once

#include "ArcGeoSim/Numerics/AuDi/audi.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

namespace ArcNum {

  using Contribution = ::audi::sparse_expression<Arcane::Real>;
  using RootContribution = ::audi::scalar_wrapper<Arcane::Real>;
  using DenseContribution = ::audi::scalar<Arcane::Real>;
  constexpr auto initContribution = &::audi::pool_reset<Arcane::Real>;

}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
