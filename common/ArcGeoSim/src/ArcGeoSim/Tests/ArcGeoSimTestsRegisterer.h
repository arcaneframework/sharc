// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*/
/* ArcGeoSimTestsRegisterer.h                                  (C) 2000-2011 */
/*                                                                           */
/* An empty class used to load a symbol from this library.                   */
/* Very useful under Windows.                                                */
/*---------------------------------------------------------------------------*/
#ifndef ARGEOSIM_TESTS_ARCGEOSIMTESTSREGISTER_H
#define ARGEOSIM_TESTS_ARCGEOSIMTESTSREGISTER_H
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef ARCGEOSIM_USE_EXPORT
#include "ArcGeoSim/Tests/ArcGeoSim_testsExport.h"
#else
#define ARCGEOSIM_TESTS_EXPORT
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class ARCGEOSIM_TESTS_EXPORT ArcGeoSimTestsRegisterer
{
 public:

  ArcGeoSimTestsRegisterer();
  virtual ~ArcGeoSimTestsRegisterer();

 public:

  static void registerLibrary();
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif


