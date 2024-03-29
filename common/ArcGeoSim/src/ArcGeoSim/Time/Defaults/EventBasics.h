// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
#ifndef ARCGEOSIM_ARCGEOSIM_TIME_DEFAULTS_EVENTBASICS_H
#define ARCGEOSIM_ARCGEOSIM_TIME_DEFAULTS_EVENTBASICS_H
/* Author : desrozis at Thu Oct 30 16:54:25 2014
 * Generated by createNew
 */

#include "ArcGeoSim/Time/IEvent.h"
#include "ArcGeoSim/Time/TimeVariables.h"

#include <arcane/utils/TraceAccessor.h>

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

BEGIN_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class EventBasics
  : public IEvent
  , protected Arcane::TraceAccessor
  , protected TimeVariables
{
public:
  
  EventBasics(Arcane::String name,
              Arcane::Real date,
              Arcane::Integer entry_points,
              Arcane::Integer types = 0);

  EventBasics(Arcane::String name,
              Arcane::Real start_date,
              Arcane::Real end_date,
              Arcane::Integer entry_points,
              Arcane::Integer types = 0);

public:

  virtual ~EventBasics() {}
  
public:

  Arcane::String name() const { return m_name; }

  Arcane::Real startDate() const { return m_start_date; }

  Arcane::Real endDate() const { return m_end_date; }

  void attachObservable(TimeStep::IObservable* observable);

  void notify(const TimeStep::EntryPoint& entry_point);

  virtual void execute() = 0;

protected:

  Arcane::Integer entryPoints() const { return m_entry_points; }

  Arcane::Integer types() const { return m_types; }

protected:

  Arcane::String m_name;
  Arcane::Real m_start_date;
  Arcane::Real m_end_date;
  Arcane::Integer m_entry_points;
  Arcane::Integer m_types;

  TimeStep::IObservable* m_observable;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

END_ARCGEOSIM_NAMESPACE

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#endif /* ARCGEOSIM_ARCGEOSIM_TIME_DEFAULTS_EVENTBASICS_H */
