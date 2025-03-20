// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#ifndef ARCGEOSIM_TESTS_SERVICETESTMNG_SERVICETESTMNGMODULE_H
#define ARCGEOSIM_TESTS_SERVICETESTMNG_SERVICETESTMNGMODULE_H

#include "ArcGeoSim/Utils/ArcGeoSim.h"
#include "ArcGeoSim/Tests/IServiceValidator.h"
#include "ArcGeoSim/Tests/ServiceTesters/IServiceTester.h"
#include "ArcGeoSim/Tests/ServiceTestMng/BaseUnitTestMng.h"

#include "ServiceTestMng_axl.h"

namespace Arcane { }
using namespace Arcane;

class ServiceTestMngModule
: public ArcaneServiceTestMngObject
  , public IAppServiceMng
  , public ArcGeoSim::BaseUnitTestMng<ServiceTestMngModule>
  //  , private TimeMngBase
{
 public:
  typedef  ArcGeoSim::BaseUnitTestMng<ServiceTestMngModule> BaseType ;

  /** Constructeur de la classe */
  ServiceTestMngModule(const Arcane::ModuleBuildInfo& mbi)
    : ArcaneServiceTestMngObject(mbi)
    , IAppServiceMng()
    , BaseType(this)
      //, TimeMngBase(mbi.m_sub_domain)
    , m_initialized(false)
   {
   }

  /** Destructeur de la classe */
  virtual ~ServiceTestMngModule() {}

 protected:
     void initializeAppServiceMng();

 public:
  virtual void init();
  
  virtual void test() {
    BaseType::test() ;
  }

  /** Retourne le num�ro de version du module */
  virtual Arcane::VersionInfo versionInfo() const { return Arcane::VersionInfo(1,0,0); }
private:
  bool m_initialized ;
};

#endif /* ARCGEOSIM_TESTS_SERVICETESTMNG_SERVICETESTMNGMODULE_H */
