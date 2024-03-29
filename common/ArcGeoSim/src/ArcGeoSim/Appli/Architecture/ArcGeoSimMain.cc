// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ArcGeoSimMain.h"
/* Author : desrozis at Fri Jun 12 14:15:10 2015
 * Generated by createNew
 */

#include <arcane/ArcaneVersion.h>
#include <arcane/impl/ArcaneMain.h>
#include <arcane/impl/MainFactory.h>
#include <arcane/IModuleMng.h>
#include <arcane/ISubDomain.h>

#include "ArcGeoSim/Appli/Architecture/ModuleMaster.h"

namespace Arcane {
extern "C++" IArcaneMain*
createArcaneMainBatch(const ApplicationInfo& exe_info,IMainFactory*);
}

namespace ArcGeoSim {

  class MainFactory 
    : public Arcane::MainFactory 
  {
  public:
    
    MainFactory(Arcane::String code_name)
      : m_code_name(Arcane::String::format("Master{0}",code_name)) {}
    
    virtual ~MainFactory() {}
    
    Arcane::IModuleMaster* createModuleMaster(Arcane::ISubDomain* sd) {
      Arcane::ModuleBuildInfo mbi(sd,sd->defaultMesh(),m_code_name);
      ArcGeoSim::ModuleMaster* master = new ArcGeoSim::ModuleMaster(mbi);
#if (ARCANE_VERSION >= 22200)
      sd->moduleMng()->addModule(makeRef(master));
#else
      sd->moduleMng()->addModule(master);
#endif
      return master;
    }
    
    Arcane::IArcaneMain* createArcaneMain(const Arcane::ApplicationInfo& info) {
      return createArcaneMainBatch(info,this);
    }
    
  private:
    
    const Arcane::String m_code_name;
  };

  
  int main(Arcane::ApplicationInfo& info) {
   
    Arcane::ArcaneMain::arcaneInitialize();

    ArcGeoSim::MainFactory factory(info.applicationName());
    
    int r = Arcane::ArcaneMain::arcaneMain(info,&factory);
    
    Arcane::ArcaneMain::arcaneFinalize();
    
    return r; 
  }

}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
