// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_EVENTCALENDAR_H
#define ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_EVENTCALENDAR_H
/* Author : desrozis at Wed Oct 29 09:34:33 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Time/IEventCalendar.h"

#include "ArcGeoSim/Time/IEvent.h"
#include "ArcGeoSim/Time/TimeVariables.h"
#include "ArcGeoSim/Time/Observer/ObserverSet.h"

#include <arcane/utils/TraceAccessor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class EventCalendar
  : public IEventCalendar
  , private Arcane::TraceAccessor
  , private TimeVariables
  , public TimeStep::IObservable
  , public TimeStep::IObserver
{
public:
  
  EventCalendar(Arcane::ISubDomain* sd);
  
  virtual ~EventCalendar();
  
public:

  //!@{ IEventCalendar
  void schedule(IEvent* event, Ownership::ePolicy owner = Ownership::Take);

  void execute(const TimeStep::EntryPoint& entry_point);
  
  void print() const;
  //!}

  //!@{ IObserver
  void attachObservable(TimeStep::IObservable* observable);

  void notify(const TimeStep::EntryPoint& entry_point);
  
  void notify(eTimeStepState state);
  //!}
  
  //!@{ IObservable
  void attachObserver(TimeStep::IObserver* observer);
 
  void detachObserver(TimeStep::IObserver* observer);
  //!}
  
  Arcane::Real minTime() const;
  Arcane::Real maxTime() const;
  
  Arcane::Real nextEventTime();

  void setVerbosity(const bool verbosity)
  {
    m_verbose = verbosity;
  }

private:

  bool m_verbose;

  Arcane::SharedArray<IEvent*> m_ownership_events;

  TimeStep::ObserverSet m_observers;

  TimeStep::IObservable* m_observable;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_TIMELINE_EVENTCALENDAR_H */
