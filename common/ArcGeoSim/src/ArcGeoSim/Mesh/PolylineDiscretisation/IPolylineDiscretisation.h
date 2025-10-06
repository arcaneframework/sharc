// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_MESH_MESH_POLYLINEDISCRETISATION_IPOLYLINEDISCRETISATION_H
#define ARCGEOSIM_MESH_MESH_POLYLINEDISCRETISATION_IPOLYLINEDISCRETISATION_H


#include <arcane/IMesh.h>
#include "ArcGeoSim/Mesh/AccessorToolsMng/IMeshAccessorToolsMng.h"
#include "ArcGeoSim/Mesh/PolylineDiscretisation/Utils/Polyline.h"
#include "ArcGeoSim/Mesh/PolylineDiscretisation/PolylineDiscretisationImpl/PolylineDiscretisationData.h"

using namespace Arcane;


class IPolylineDiscretisation
{
public:

  /** Destructeur de la classe */
  virtual ~IPolylineDiscretisation()
  {
  }

public:
  //! Compute service function
  virtual void init(ArcGeoSim::IMeshAccessorToolsMng* mng) = 0;
  //! Compute service function
  virtual void compute(PolylineArray& polylineList,PolylineDiscretisationDataArray& result) = 0;
};

#endif /* ARCGEOSIM_MESH_MESH_POLYLINEDISCRETISATION_IPOLYLINEDISCRETISATION_H */
