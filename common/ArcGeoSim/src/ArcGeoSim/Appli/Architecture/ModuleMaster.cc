// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
#include "ModuleMaster.h"
/* Author : desrozis at Wed Oct 15 15:28:46 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Appli/Architecture/BackwardManager.h"
#include "ArcGeoSim/Appli/Architecture/Delegate.h"
#include "ArcGeoSim/Appli/Architecture/INextTimeComputer.h"
#include "ArcGeoSim/Appli/Architecture/IIterationInformation.h"
#include "ArcGeoSim/Appli/Architecture/IExtraEntryPoints.h"

#include <arcane/EntryPoint.h>
#include <arcane/ISubDomain.h>
#include <arcane/ITimeLoopMng.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

ModuleMaster::
ModuleMaster(const Arcane::ModuleBuildInfo& mbi)
  : Arcane::ModuleMaster(mbi)
  , m_backward_mng(NULL)
{
  addEntryPoint(this,"ArcGeoSimMasterFirstBuild",
                &ModuleMaster::_arcgeosimFirstBuild,
                Arcane::IEntryPoint::WBuild,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastBuild",
                &ModuleMaster::_arcgeosimLastBuild,
                Arcane::IEntryPoint::WBuild,
                Arcane::IEntryPoint::PAutoLoadEnd);

  addEntryPoint(this,"ArcGeoSimMasterFirstStartInit",
                &ModuleMaster::_arcgeosimFirstStartInit,
                Arcane::IEntryPoint::WStartInit,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastStartInit",
                &ModuleMaster::_arcgeosimLastStartInit,
                Arcane::IEntryPoint::WStartInit,
                Arcane::IEntryPoint::PAutoLoadEnd);
 
  addEntryPoint(this,"ArcGeoSimMasterFirstContinueInit",
                &ModuleMaster::_arcgeosimFirstContinueInit,
                Arcane::IEntryPoint::WContinueInit,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastContinueInit",
                &ModuleMaster::_arcgeosimLastContinueInit,
                Arcane::IEntryPoint::WContinueInit,
                Arcane::IEntryPoint::PAutoLoadEnd);

  addEntryPoint(this,"ArcGeoSimMasterFirstInit",
                &ModuleMaster::_arcgeosimFirstInit,
                Arcane::IEntryPoint::WInit,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastInit",
                &ModuleMaster::_arcgeosimLastInit,
                Arcane::IEntryPoint::WInit,
                Arcane::IEntryPoint::PAutoLoadEnd);

  addEntryPoint(this,"ArcGeoSimMasterFirstComputeLoop",
                &ModuleMaster::_arcgeosimFirstComputeLoop,
                Arcane::IEntryPoint::WComputeLoop,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastComputeLoop",
                &ModuleMaster::_arcgeosimLastComputeLoop,
                Arcane::IEntryPoint::WComputeLoop,
                Arcane::IEntryPoint::PAutoLoadEnd);

  addEntryPoint(this,"ArcGeoSimMasterFirstOnMeshChanged",
                &ModuleMaster::_arcgeosimFirstOnMeshChanged,
                Arcane::IEntryPoint::WOnMeshChanged,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastOnMeshChanged",
                &ModuleMaster::_arcgeosimLastOnMeshChanged,
                Arcane::IEntryPoint::WOnMeshChanged,
                Arcane::IEntryPoint::PAutoLoadEnd);

  addEntryPoint(this,"ArcGeoSimMasterFirstRestore",
                &ModuleMaster::_arcgeosimFirstRestore,
                Arcane::IEntryPoint::WRestore,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastRestore",
                &ModuleMaster::_arcgeosimLastRestore,
                Arcane::IEntryPoint::WRestore,
                Arcane::IEntryPoint::PAutoLoadEnd);

  addEntryPoint(this,"ArcGeoSimMasterFirstExit",
                &ModuleMaster::_arcgeosimFirstExit,
                Arcane::IEntryPoint::WExit,
                Arcane::IEntryPoint::PAutoLoadBegin);
  addEntryPoint(this,"ArcGeoSimMasterLastExit",
                &ModuleMaster::_arcgeosimLastExit,
                Arcane::IEntryPoint::WExit,
                Arcane::IEntryPoint::PAutoLoadEnd);
}

/*---------------------------------------------------------------------------*/

ModuleMaster::
~ModuleMaster()
{
  delete m_backward_mng;
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
timeIncrementation()
{
  if(Delegate<INextTimeComputer>::isEnabled())
    Delegate<INextTimeComputer>::delegate(*this).timeIncrementation();
  else
    Arcane::ModuleMaster::timeIncrementation();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
timeStepInformation() 
{
  if(Delegate<IIterationInformation>::isEnabled())
    Delegate<IIterationInformation>::delegate(*this).timeStepInformation(m_nb_loop);
  else
    Arcane::ModuleMaster::timeStepInformation();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstBuild()
{
  if(Delegate<IExtraEntryPoints>::isEnabled()) {
    m_backward_mng = new BackwardManager(subDomain());
    subDomain()->timeLoopMng()->setBackwardMng(m_backward_mng);
    
    Delegate<IExtraEntryPoints>::delegate(*this).firstBuild();
  }
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastBuild()
{
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastBuild();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstStartInit()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstStartInit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastStartInit()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastStartInit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstContinueInit()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstContinueInit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastContinueInit()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastContinueInit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstInit()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstInit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastInit()
{
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastInit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstComputeLoop()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstComputeLoop();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastComputeLoop()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastComputeLoop();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstRestore()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstRestore();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastRestore()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastRestore();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstExit()
{
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstExit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastExit()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastExit();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimFirstOnMeshChanged()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).firstOnMeshChanged();
}

/*---------------------------------------------------------------------------*/

void
ModuleMaster::
_arcgeosimLastOnMeshChanged()
{  
  if(Delegate<IExtraEntryPoints>::isEnabled())
    Delegate<IExtraEntryPoints>::delegate(*this).lastOnMeshChanged();
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
