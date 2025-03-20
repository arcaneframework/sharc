// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_NUMERICS_LINEARALGEBRA2_CREATEDISTRIBUTION_H
#define ARCGEOSIM_NUMERICS_LINEARALGEBRA2_CREATEDISTRIBUTION_H


#include "ArcGeoSim/Numerics/AlienTools/IIndexManager.h"
#include "ArcGeoSim/Numerics/AlienTools/Space.h"

#ifdef USE_ALIEN_V2
#include <alien/distribution/VectorDistribution.h>
#include <alien/distribution/MatrixDistribution.h>
#else
#include <ALIEN/Distribution/VectorDistribution.h>
#include <ALIEN/Distribution/MatrixDistribution.h>
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(Alien)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_NAMESPACE(ArcGeoSim)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

inline 
VectorDistribution
createVectorDistribution(Space& space)
{
  auto* im = space.indexManager();
  
  return Alien::VectorDistribution(space, im->localSize(), im->parallelMng());
}

/*---------------------------------------------------------------------------*/

inline 
MatrixDistribution
createMatrixDistribution(Space& space)
{
  auto* im = space.indexManager();
  
  return Alien::MatrixDistribution(space, space, im->localSize(), im->parallelMng());
}

/*---------------------------------------------------------------------------*/

inline 
VectorDistribution
createVectorDistribution(Space& space, Arcane::Integer block_size)
{
  auto* im = space.indexManager();
  
  return Alien::VectorDistribution(space, im->localSize()/block_size, im->parallelMng());
}

/*---------------------------------------------------------------------------*/

inline 
MatrixDistribution
createMatrixDistribution(Space& space, Arcane::Integer block_size)
{
  auto* im = space.indexManager();
  
  return Alien::MatrixDistribution(space, space, im->localSize()/block_size, im->parallelMng());
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_NUMERICS_LINEARALGEBRA2_CREATEDISTRIBUTION_H */
