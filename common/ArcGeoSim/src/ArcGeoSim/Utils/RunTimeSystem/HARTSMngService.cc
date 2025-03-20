// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------

#include "ArcGeoSim/Utils/Utils.h"

#ifdef USE_HARTS
#include "HARTS/HARTS.h"
#endif

#include "ArcGeoSim/Utils/RunTimeSystem/IRunTimeSystemMng.h"
#include "ArcGeoSim/Utils/RunTimeSystem/HARTSThreadEnv.h"
#include "ArcGeoSim/Utils/RunTimeSystem/HARTSTaskImplementation.h"
#include "ArcGeoSim/Utils/RunTimeSystem/HARTSMngService.h"

#include <arcane/utils/PlatformUtils.h>

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
HARTSMngService::
init()
{
  if(m_is_initialized) return ;
  HARTSTaskImplementation* task_mng = HARTSTaskImplementation::instance() ;
  if(task_mng==NULL)
  {
    Integer nb_threads = options()->nbThread() ;
    String nb_threads_str = platform::getEnvironmentVariable("ARCANE_NB_THREAD") ;
    if(!nb_threads_str.null())
    {
      Integer nbth = nb_threads ;
      bool is_bad = builtInGetValue(nb_threads,nb_threads_str);
      if(!is_bad && nbth>0)
        nb_threads = nbth ;
    }
    info()<<"USING HARTS WITH "<<nb_threads<<" THREADS";
    m_thread_env = new ArcGeoSim::HARTSThreadEnv(nb_threads) ;
  }

  m_is_initialized = true ;
}


ArcGeoSim::IThreadEnv* HARTSMngService::getThreadEnv() {
  HARTSTaskImplementation* task_mng = HARTSTaskImplementation::instance() ;
  if(task_mng)
    return task_mng->getThreadEnv() ;
  else
    return m_thread_env ;
}



ARCANE_REGISTER_SERVICE_HARTSMNG(HARTSMng,HARTSMngService);
