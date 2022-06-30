// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef PHYSICS_CONDITIONS_IINITIALCONDITION_H
#define PHYSICS_CONDITIONS_IINITIALCONDITION_H
/* Author : desrozis at Mon Aug 20 10:10:51 2012
 * Generated by createNew
 */

#include "ArcRes/Entities/System.h"

#include "ArcGeoSim/Physics/Law2/Contribution/VariableFolderAccessor.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class IInitialCondition
{
public:
  
  virtual ~IInitialCondition() {}
  
public:

  virtual void init(ArcRes::System& system ,Law::VariableCellFolder& domain) = 0;
  
  virtual void revaluate(ArcRes::System& system ,Law::VariableCellFolder& domain) = 0;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* PHYSICS_CONDITIONS_IINITIALCONDITION_H */
