// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#include "ArcGeoSim/Utils/Utils.h"
#include "ArcGeoSim/Utils/PropertyHolder.h"
#include "IUnitsSystem.h"
#include "SIUnitsSystem.h"

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Real
SIUnitsSystemService::
convertTo(IUnitsSystem* system,IUnitsSystem::UnitType unit, Real val)
{
  return getMultFactorTo(system,unit)*val + getConstFactorTo(system,unit);
}

void 
SIUnitsSystemService::
convertTo(IUnitsSystem* system,IUnitsSystem::UnitType unit, ArrayView<Real> buf)
{
  Real* ptr = buf.unguardedBasePointer() ;
  const Real factor = getMultFactorTo(system,unit) ;
  const Real const_factor = getConstFactorTo(system,unit) ;
  for(Integer i=0;i<buf.size();i++)
      ptr[i] = const_factor+factor*ptr[i] ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_SERVICE_SIUNITSSYSTEM(SIUnitsSystem,SIUnitsSystemService);
