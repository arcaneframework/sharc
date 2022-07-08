// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSim/Appli/Architecture/ArcGeoSimMain.h"

#ifdef ARCGEOSIM_USE_EXPORT
#include "DemonstratorDynamicLoading.h"
#endif

int main(int argc, char* argv[])
{ 
  Arcane::ApplicationInfo info(&argc,&argv,
                               "Demonstrator",
                               Arcane::VersionInfo(1,0,0));
  
  return ArcGeoSim::main(info);
}
