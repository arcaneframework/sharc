// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Numerics/AlienTools/VectorAccessor.h"

/*---------------------------------------------------------------------------*/

#define ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSOR_CC

// Si on est en debug ou qu'on ne souhaite pas l'inlining, VectorAccessorT est inclus ici
#ifdef ARCGEOSIM_LINEARALGEBRA2_INCLUDE_TEMPLATE_IN_CC
#include "ArcGeoSim/Numerics/AlienTools/VectorAccessorT.h"
#endif /* SHOULD_BE_INCLUDE_IN_CC */

#undef ARCGEOSIM_ARCGEOSIM_NUMERICS_LINEARALGEBRA2_ACCESSOR_VECTORACCESSOR_CC

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/

template class VectorAccessorBase<double>;
template class ItemVectorAccessorT<double>;

/*---------------------------------------------------------------------------*/

END_ALIEN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
