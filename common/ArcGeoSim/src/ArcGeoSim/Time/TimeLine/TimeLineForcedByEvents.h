// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_TIMELINEFORCEDBYEVENTS_H
#define ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_TIMELINEFORCEDBYEVENTS_H
/* Author : desrozis at Wed May 27 14:30:02 2015
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#include "ArcGeoSim/Time/ITimeLine.h"
#include "ArcGeoSim/Time/TimeLine/SpiedTimeVariables.h"
#include "ArcGeoSim/Time/TimeLine/EventCalendar.h"
#include "ArcGeoSim/Time/TimeLine/TimeEvolution.h"
#include "ArcGeoSim/Time/Defaults/TimeStepModifier.h"
#include "ArcGeoSim/Time/Observer/ObserverSet.h"

#include "ArcGeoSim/Physics/Units/SIUnitsConverter.h"

#include "ArcGeoSim/Appli/AppService.h"
#include "ArcGeoSim/Appli/Architecture/Delegate.h"
#include "ArcGeoSim/Appli/Architecture/INextTimeComputer.h"
#include "ArcGeoSim/Appli/Architecture/IExtraEntryPoints.h"

#include "ArcGeoSim/Utils/Optional.h"
#include "ArcGeoSim/Utils/Spy.h"
#include "ArcGeoSim/Utils/OutputFormatting/OutputFormatting.h"

#include <arcane/ITimeLoopMng.h>
#include <arcane/utils/TraceAccessor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class TimeLineForcedByEvents
  : public Arcane::TraceAccessor
  , public ITimeLine
  , public SpiedTimeVariables
  , public Delegate<INextTimeComputer>
  , public Delegate<IExtraEntryPoints>
{
public:
  
  TimeLineForcedByEvents(Arcane::ISubDomain* sub_domain);
  
  ~TimeLineForcedByEvents() {}
  
public:

  void init();

  //!@{ ITimeLine
  bool isStepReplayed() const { return false; }
  
  ITimeEvolution& evolution() { return m_evolution; }
  
  IEventCalendar& calendar() { return m_calendar; }
  //!}

  //!@{ Delegate<INextTimeComputer>
  TimeStep::EntryPoints& userEntryPoints() { return m_entry_points; }

  void notify(const TimeStep::EntryPoint& entry_point);

  // Delegation of time incrementation of arcane time loop
  void timeIncrementation();
  //!}

  //!@{ Delegate<IExtraEntryPoints>
  // Delegation of extra entry points
  // ie automatic position of time tools in
  // arcane time loop
  void firstInit();
  void lastInit();
  void firstComputeLoop();
  void lastComputeLoop();
  void firstContinueInit();
  void firstRestore();

  // If another ITimeLine is setted, maybe not the delegated
  void claimDelegation() 
  {
    Delegate<INextTimeComputer>::claimDelegation();
    Delegate<IExtraEntryPoints>::claimDelegation();
  }
  //!}
  
  //!@{ IObservable
  void attachObserver(TimeStep::IObserver* observer);
  
  void detachObserver(TimeStep::IObserver* observer);
  //!}

private:

  void _computeTimeStep();

private:

  bool m_initialized;

  Arcane::ISubDomain* m_sub_domain;

  EventCalendar m_calendar;
  TimeEvolution m_evolution;

  Arcane::VariableScalarByte m_final_time_is_reached;

  Arcane::VariableScalarReal m_next_time;

  Arcane::VariableScalarReal m_dt_n;

  TimeStep::EntryPoints m_entry_points;

  TimeStep::ObserverSet m_observers;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_TIMELINEFORCEDBYEVENTS_H */
