// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ArcGeoSimAppliRegisterer.h                                  (C) 2000-2011 */
/*                                                                           */
/* An empty class used to load a symbol from this library.                   */
/* Very useful under Windows.                                                */
/*---------------------------------------------------------------------------*/
#ifndef ARGEOSIM_APPLI_ARCGEOSIMAPPLIREGISTER_H
#define ARGEOSIM_APPLI_ARCGEOSIMAPPLIREGISTER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Appli/ArcGeoSim_appliExport.h"
#else
#define ARCGEOSIM_APPLI_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ARCGEOSIM_APPLI_EXPORT ArcGeoSimAppliRegisterer
{
 public:

  ArcGeoSimAppliRegisterer();
  virtual ~ArcGeoSimAppliRegisterer();

 public:

  static void registerLibrary();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif


