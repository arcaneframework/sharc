// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ArcGeoSimUtilsRegisterer.h                                  (C) 2000-2011 */
/*                                                                           */
/* An empty class used to load a symbol from this library.                   */
/* Very useful under Windows.                                                */
/*---------------------------------------------------------------------------*/
#ifndef ARGEOSIM_UTILS_ARCGEOSIMUTILSREGISTER_H
#define ARGEOSIM_UTILS_ARCGEOSIMUTILSREGISTER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Utils/ArcGeoSim_utilsExport.h"
#else
#define ARCGEOSIM_UTILS_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ARCGEOSIM_UTILS_EXPORT ArcGeoSimUtilsRegisterer
{
 public:

  ArcGeoSimUtilsRegisterer();
  virtual ~ArcGeoSimUtilsRegisterer();

 public:

  static void registerLibrary();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif


