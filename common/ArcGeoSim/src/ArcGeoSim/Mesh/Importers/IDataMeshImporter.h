// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef IDATAMESHIMPORTER_H
#define IDATAMESHIMPORTER_H

/**
 * Interface for User date importer classes
 * helper to convert data with imput file unit system
 */

#include "ArcGeoSim/Utils/ArcGeoSim.h"

class IUnitsSystem ;

BEGIN_ARCGEOSIM_NAMESPACE

BEGIN_MESH_NAMESPACE

class IDataMeshImporter
{
public :
  IDataMeshImporter() {}
  virtual ~IDataMeshImporter() {}
  
  virtual void setDataImportUnitsSystem(IUnitsSystem* system) = 0 ;
};

END_MESH_NAMESPACE

END_ARCGEOSIM_NAMESPACE

#endif /*IDATAMESHIMPORTER_*/
