// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Utils/Utils.h"
#include "NotMatchingSurfaceProperty.h"

#include <arcane/utils/FatalErrorException.h>
using namespace Arcane;

/*---------------------------------------------------------------------------*/

String 
NotMatchingSurfaceProperty::
name(const eProperty p) 
{
  switch (p) {
  case PNone:
    return "None";
  case PSimpleCorefinementArea:
    return "SimpleCorefinementArea";
  case PSimpleCorefinementCenter:
    return "SimpleCorefinementCenter";
  case PSimpleCorefinementNormal:
    return "SimpleCorefinementNormal";
  case PDoubleCorefinementArea:
    return "DoubleCorefinementArea";
  case PDoubleCorefinementCenter:
    return "DoubleCorefinementCenter";
  case PDoubleCorefinementNormal:
    return "DoubleCorefinementNormal";
  case PDoubleCorefinementDistance:
    return "DoubleCorefinementDistance";
  case PProjectedPoint:
    return "ProjectedPoint";
  case PProjectedDistance:
    return "ProjectedDistance";
  case PCorefinementUserProperty:
    return "CorefinementUserProperty";
  case PProjectionUserProperty:
    return "ProjectionUserProperty";
  case PIntegerContactZoningKey:
    return "PIntegerContactZoningKey";
  case PIntegerFaceZoningKey:
    return "PIntegerFaceZoningKey";
  case PEnd:
  default:
    throw FatalErrorException(A_FUNCINFO,"Undefined property");
    return String();
  }
}

/*---------------------------------------------------------------------------*/

bool
NotMatchingSurfaceProperty::
check(const Integer property)
{
  // Devrait etre �gal = (Integer(PEnd)-1)
  const Integer fullMask = 
    PSimpleCorefinementArea
    |PSimpleCorefinementCenter
    |PSimpleCorefinementNormal
    |PDoubleCorefinementArea
    |PDoubleCorefinementCenter
    |PDoubleCorefinementNormal
    |PDoubleCorefinementDistance
    |PProjectedPoint
    |PProjectedDistance
    |PCorefinementUserProperty
    |PProjectionUserProperty
    |PIntegerContactZoningKey
    |PIntegerFaceZoningKey
    ;
  return ((property & ~fullMask) == 0);
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
