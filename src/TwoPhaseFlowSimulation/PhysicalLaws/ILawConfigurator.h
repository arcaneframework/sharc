// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef GEOXIM_GEOXIMLAWSET_ILAWCONFIGURATOR_H
#define GEOXIM_GEOXIMLAWSET_ILAWCONFIGURATOR_H
/* Author : desrozis at Fri Jan 13 16:41:36 2012
 * Generated by createNew
 */

#include "ArcRes/Entities/System.h"
#include "ArcGeoSim/Physics/Law2/FunctionManager.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ILawConfigurator
{
public:

  virtual ~ILawConfigurator() {}
  
public:

  virtual void configure(Law::FunctionManager& law_register, ArcRes::System& system) = 0;

};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* GEOXIM_GEOXIMLAWSET_ILAWCONFIGURATORTEST_H */
