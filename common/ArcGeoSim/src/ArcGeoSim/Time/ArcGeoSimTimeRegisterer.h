// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ArcGeoSimTimeRegisterer.h                                   (C) 2000-2011 */
/*                                                                           */
/* An empty class used to load a symbol from this library.                   */
/* Very useful under Windows.                                                */
/*---------------------------------------------------------------------------*/
#ifndef ARGEOSIM_TIME_ARCGEOSIMTIMEREGISTER_H
#define ARGEOSIM_TIME_ARCGEOSIMTIMEREGISTER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Time/ArcGeoSim_timeExport.h"
#else
#define ARCGEOSIM_TIME_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ARCGEOSIM_TIME_EXPORT ArcGeoSimTimeRegisterer
{
 public:

  ArcGeoSimTimeRegisterer();
  virtual ~ArcGeoSimTimeRegisterer();

 public:

  static void registerLibrary();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif


