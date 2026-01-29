// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ArcGeoSimNumericsRegisterer.h                               (C) 2000-2011 */
/*                                                                           */
/* An empty class used to load a symbol from this library.                   */
/* Very useful under Windows.                                                */
/*---------------------------------------------------------------------------*/
#ifndef ARGEOSIM_NUMERICS_ARCGEOSIMNUMERICSREGISTER_H
#define ARGEOSIM_NUMERICS_ARCGEOSIMNUMERICSREGISTER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Numerics/ArcGeoSim_numericsExport.h"
#else
#define ARCGEOSIM_NUMERICS_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ARCGEOSIM_NUMERICS_EXPORT ArcGeoSimNumericsRegisterer
{
 public:

  ArcGeoSimNumericsRegisterer();
  virtual ~ArcGeoSimNumericsRegisterer();

 public:

  static void registerLibrary();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif


