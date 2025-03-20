// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#include "ArcGeoSim/Appli/IAppServiceMng.h"
#include "ArcGeoSim/Mesh/Geometry/IGeometryMng.h"
#include "ArcGeoSim/Numerics/Expressions/IExpressionMng.h"
#include "ArcGeoSim/Utils/RunTimeSystem/IRunTimeSystemMng.h"

#include "ServiceTestMngModule.h"

#include <arcane/ITimeLoopMng.h>


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#define ADD_OPTIONAL_SERVICE(SERVICE_NAME,STRING_NAME)                  \
  if( options()->SERVICE_NAME.size() > 0 )                              \
    addService(options()->SERVICE_NAME[0]); \
  else                                                                  \
    addMissingServiceInfo( typeid(options()->SERVICE_NAME[0]).name(), STRING_NAME );  \

void
ServiceTestMngModule::
initializeAppServiceMng()
{

  if (m_initialized)
    return;
  info() << "Initializing service manager";

  ADD_OPTIONAL_SERVICE(geometryService,"Geometry");
  ADD_OPTIONAL_SERVICE(expressionMng,"Expression Manager");
  ADD_OPTIONAL_SERVICE(runtimeSystemMng,"RunTimeSystem Manager");

  m_initialized = true ;
}

void ServiceTestMngModule::init() {
  info() << "init";

  initializeAppServiceMng();

//#ifndef NO_USER_WARNING
//#ifndef WIN32
//#warning "TODO: enforce policy about geometry update"
//#endif
//#endif
  // This is a application initialization module, so it must define the policy
  IAppServiceMng * app_service_mng = IAppServiceMng::instance(subDomain()->serviceMng());
  IGeometryMng * geometry = app_service_mng->find<IGeometryMng>(false);
  if (geometry)
    geometry->setPolicyTolerance(true);

  this->options()->service[0]->init();
}


using namespace Arcane;
ARCANE_REGISTER_MODULE_SERVICETESTMNG(ServiceTestMngModule);
