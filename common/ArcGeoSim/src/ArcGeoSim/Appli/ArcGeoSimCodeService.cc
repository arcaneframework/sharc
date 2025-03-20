// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2025 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
/* ArcGeoSimCodeService.cc                                        (C) 2000-2011 */
/*                                                                           */
/* Service de code générique Arcane.                                         */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifdef WIN32
#include <ciso646>
#endif

#ifdef WIN32
#include <direct.h>
// MSDN recommends against using chdir names
#define cd _chdir
#else
#include "unistd.h"
#define cd chdir
#endif

#include <arcane/utils/ArcanePrecomp.h>

#include <arcane/utils/ITraceMng.h>
#include <arcane/utils/StdHeader.h>
#include <arcane/utils/PlatformUtils.h>

#include <arcane/IApplication.h>
#include <arcane/ISession.h>
#include <arcane/ISubDomain.h>
#include <arcane/IParallelMng.h>
#include <arcane/Service.h>

#include <arcane/impl/TimeLoopReader.h>

#include <arcane/std/ArcaneSession.h>

#include <ArcGeoSim/Utils/ArcGeoSim.h>
#include <ArcGeoSim/Appli/ArcGeoSimCodeService.h>


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

using namespace Arcane;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ArcGeoSimCodeService::
ArcGeoSimCodeService(const ServiceBuildInfo& sbi)
  : CodeService(sbi)
  , m_case_name("output")
{
#if (ARCANE_VERSION >= 11603)
  // Introduction de la s�paration du ArcaneCodeService dans 1.16.3
  _addExtension(String("arc"));
#endif /* ARCANE_VERSION */
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ArcGeoSimCodeService::
~ArcGeoSimCodeService()
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void ArcGeoSimCodeService::
_preInitializeSubDomain(ISubDomain* sd)
{
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void ArcGeoSimCodeService::
initCase(ISubDomain* sub_domain,bool is_continue)
{
  {
    TimeLoopReader stl(_application());
    stl.readTimeLoops();
    stl.registerTimeLoops(sub_domain);
    stl.setUsedTimeLoop(sub_domain);
  }
  CodeService::initCase(sub_domain,is_continue);
  if (sub_domain->parallelMng()->isMasterIO())
    sub_domain->session()->writeExecInfoFile();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ISession* ArcGeoSimCodeService::
createSession()
{
	ArcaneSession * session = new ArcaneSession(_application());
#if (ARCANE_VERSION >= 11603)
	session->setCaseName(m_case_name);
#endif /* ARCANE_VERSION */
#if (ARCANE_VERSION >= 12102)
	// to remove when arcane use this environment variable directly
	const String env_full_output_path = platform::getEnvironmentVariable("ARCANE_OUTPUT_ROOT_PATH");
	if(env_full_output_path.null()){
		session->build();
	}
	else{
		const String current_dir = platform::getCurrentDirectory();
		cd(env_full_output_path.localstr());
		session->build();
		cd(current_dir.localstr());
	}
#endif /* ARCANE_VERSION */
	_application()->addSession(session);
	return session;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

bool ArcGeoSimCodeService::
parseArgs(StringList& args)
{
  // Contournement du bug de Collection::removeAt trunk@1056
#if (ARCANE_VERSION >= 11603)
  String us_casename("-casename");

  for(Integer i=0;i<args.count();) // incr�mentation dans la boucle
    {
      if (args[i] == us_casename and i+1<args.count())
        {
          m_case_name = args[i+1];
          args.removeAt(i); // remove -casename 'casename'
          args.removeAt(i);          
        }
      else
        {
          ++i;
        }
    }
#endif /* ARCANE_VERSION */

  return false;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ARCANE_REGISTER_APPLICATION_FACTORY(ArcGeoSimCodeService,ICodeService,ArcGeoSimCode);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
